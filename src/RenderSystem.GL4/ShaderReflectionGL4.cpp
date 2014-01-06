//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ShaderReflectionGL4.hpp"
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LoggingLevel.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include "../RenderSystem/EffectBufferDescription.hpp"
#include "ErrorChecker.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
namespace {
//-----------------------------------------------------------------------
static
GLint GetActiveUniformBlockIntValue(ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex, GLenum parameterName)
{
	GLint result = 0;
	glGetActiveUniformBlockiv(shaderProgram.value, uniformBlockIndex, parameterName, &result);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glGetActiveUniformBlockiv", __FILE__, __LINE__);
	#endif
	
	return std::move(result);
}
//-----------------------------------------------------------------------
static
std::string GetActiveUniformBlockName(ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex)
{
	std::string result;
	
	GLint maxUniformBlockNameLength = GetActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_NAME_LENGTH);
	std::vector<GLchar> uniformBlockName(maxUniformBlockNameLength + 1, '\0');
	
	GLsizei uniformBlockNameLength = 0;
	glGetActiveUniformBlockName(shaderProgram.value, uniformBlockIndex,
		maxUniformBlockNameLength, &uniformBlockNameLength, uniformBlockName.data());

	#ifdef DEBUG
	ErrorChecker::CheckError("glGetActiveUniformBlockName", __FILE__, __LINE__);
	#endif
	
	result.assign(uniformBlockName.data(), uniformBlockNameLength);
	
	return std::move(result);
}
//-----------------------------------------------------------------------
static
std::vector<GLuint> GetActiveUniformBlockIndices(ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex)
{
	auto const uniformCount = GetActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS);
	
	std::vector<GLint> uniformIndices(uniformCount, 0);
	glGetActiveUniformBlockiv(shaderProgram.value, uniformBlockIndex,
		GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

	#ifdef DEBUG
	ErrorChecker::CheckError("glGetActiveUniformBlockiv", __FILE__, __LINE__);
	#endif
	
	std::vector<GLuint> result(uniformIndices.begin(), uniformIndices.end());
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename Func>
static void
GetActiveUniformsIntValue(ShaderProgramGL4 const& shaderProgram, std::vector<GLuint> const& uniformIndices,
	GLenum parameter, Func func)
{
	GLsizei uniformCount = static_cast<GLuint>(uniformIndices.size()); // badcode

	std::vector<GLint> values(uniformIndices.size());
	glGetActiveUniformsiv(shaderProgram.value, uniformCount, uniformIndices.data(), parameter, values.data());
	for (GLsizei index = 0; index < uniformCount; ++index) {
		func(index, values[index]);
	}
}
//-----------------------------------------------------------------------
static
std::vector<UniformVariableGL4>
EnumerateUniformVariables(ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex)
{
	auto const uniformIndices = GetActiveUniformBlockIndices(shaderProgram, uniformBlockIndex);

	std::vector<UniformVariableGL4> uniforms(uniformIndices.size());
	
	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_OFFSET,
		[&](GLsizei index, GLint value){ uniforms[index].Offset = value; });
	
	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_SIZE,
		[&](GLsizei index, GLint value){ uniforms[index].ByteLength = value; });
	
	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_TYPE,
		[&](GLsizei index, GLint value){ uniforms[index].Type = value; });
	
	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_ARRAY_STRIDE,
		[&](GLsizei index, GLint value){ uniforms[index].ArrayStride = value; });
		
	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_MATRIX_STRIDE,
		[&](GLsizei index, GLint value){ uniforms[index].MatrixStride = value; });

	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_IS_ROW_MAJOR,
		[&](GLsizei index, GLint value){ uniforms[index].IsRowMajor = (value != 0); });

	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_NAME_LENGTH, [&](GLsizei index, GLint value)
	{
		std::vector<GLchar> uniformName(value + 1, '\0');
		GLsizei uniformNameLength = 0;
		glGetActiveUniformName(shaderProgram.value, uniformIndices[index], value, &uniformNameLength, uniformName.data());
		uniforms[index].Name.assign(uniformName.data(), uniformNameLength);
	});
	
	return std::move(uniforms);
}
//-----------------------------------------------------------------------
static
std::vector<UniformBlockGL4>
EnumerateUniformBlocks(ShaderProgramGL4 const& shaderProgram)
{
	GLint uniformBlockCount = 0;
	glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);
	
	#ifdef DEBUG
	ErrorChecker::CheckError("glGetProgramiv", __FILE__, __LINE__);
	#endif
	
	if (uniformBlockCount <= 0)
	{
		///@note have not uniform
		///@todo Not implemented
		return {};
	}
	
	std::vector<UniformBlockGL4> uniformBlocks;
	
	for (GLint index = 0; index < uniformBlockCount; ++index)
	{
		auto const uniformBlockIndex = static_cast<GLuint>(index);
		
		UniformBlockGL4 uniformBlock;
		
		uniformBlock.Name = GetActiveUniformBlockName(shaderProgram, uniformBlockIndex);
		uniformBlock.ByteConstants = GetActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE);
		//uniformBlock.BindingPoint = GetActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_BINDING);
		uniformBlock.Uniforms = EnumerateUniformVariables(shaderProgram, uniformBlockIndex);
		//uniformBlock.BlockIndex = uniformBlockIndex;
		uniformBlock.BlockIndex = glGetUniformBlockIndex(shaderProgram.value, uniformBlock.Name.data());
		
		uniformBlocks.emplace_back(std::move(uniformBlock));
	}
	
	return std::move(uniformBlocks);
}
//-----------------------------------------------------------------------
static EffectParameterType ToEffectParameterType(GLenum uniformType)
{
	switch (uniformType)
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
		return EffectParameterType::Float;

	case GL_INT: 
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return EffectParameterType::Int32;

	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
		return EffectParameterType::UInt32;

	case GL_DOUBLE:
	case GL_DOUBLE_VEC2:
	case GL_DOUBLE_VEC3:
	case GL_DOUBLE_VEC4:
	case GL_DOUBLE_MAT2:
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4:
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
		return EffectParameterType::Double;

	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		return EffectParameterType::Bool;
	
	case GL_SAMPLER_1D:
		return EffectParameterType::Texture1D;
	case GL_SAMPLER_2D:
		return EffectParameterType::Texture2D;
	case GL_SAMPLER_3D:
		return EffectParameterType::Texture3D;
	case GL_SAMPLER_CUBE:
		return EffectParameterType::TextureCube;
	
	case GL_SAMPLER_BUFFER:
		//return EffectParameterType::TextureBuffer; // See also: D3D_SVT_TBUFFER
	case GL_SAMPLER_1D_ARRAY:
		//return EffectParameterType::Texture1DArray; // See also: D3D_SVT_TEXTURE1DARRAY
	case GL_SAMPLER_2D_ARRAY:
		//return EffectParameterType::Texture2DArray; // See also: D3D_SVT_TEXTURE2DARRAY
	case GL_SAMPLER_2D_MULTISAMPLE:
		//return EffectParameterType::Texture2DMultiSample; // See also: D3D_SVT_TEXTURE2DMS
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		//return EffectParameterType::Texture2DMultiSampleArray; // See also: D3D_SVT_TEXTURE2DMSARRAY
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		// Not supported:
		POMDOG_ASSERT(uniformType);
		break;
	}

#ifdef DEBUG
	Log::Stream(LoggingLevel::Internal)
		<< "Failed to find effect parameter type '"  << uniformType << "'.";
#endif
	return EffectParameterType::Float;
}
//-----------------------------------------------------------------------
static EffectParameterClass ToEffectParameterClass(GLenum uniformType)
{
	switch (uniformType)
	{
	case GL_FLOAT:
	case GL_DOUBLE:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_BOOL:
		return EffectParameterClass::Scalar;
	
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_DOUBLE_VEC2:
	case GL_DOUBLE_VEC3:
	case GL_DOUBLE_VEC4:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		return EffectParameterClass::Vector;
	
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
	case GL_FLOAT_MAT2x3:
	case GL_FLOAT_MAT2x4:
	case GL_FLOAT_MAT3x2:
	case GL_FLOAT_MAT3x4:
	case GL_FLOAT_MAT4x2:
	case GL_FLOAT_MAT4x3:
	case GL_DOUBLE_MAT2:
	case GL_DOUBLE_MAT3:
	case GL_DOUBLE_MAT4:
	case GL_DOUBLE_MAT2x3:
	case GL_DOUBLE_MAT2x4:
	case GL_DOUBLE_MAT3x2:
	case GL_DOUBLE_MAT3x4:
	case GL_DOUBLE_MAT4x2:
	case GL_DOUBLE_MAT4x3:
		return EffectParameterClass::Matrix;

	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		return EffectParameterClass::Object;
	}

#ifdef DEBUG
	Log::Stream(LoggingLevel::Internal)
		<< "Failed to find effect parameter class '"  << uniformType << "'.";
#endif
	return EffectParameterClass::Struct;
}
//-----------------------------------------------------------------------
static void ToComponents(GLenum uniformType, std::uint8_t & RowCount, std::uint8_t ColumnCount)
{
	switch (uniformType)
	{
	case GL_FLOAT:
	case GL_DOUBLE:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_BOOL:
	case GL_BYTE: 
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		RowCount = 1; ColumnCount = 1;

	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_UNSIGNED_INT_VEC2:
	case GL_DOUBLE_VEC2:
	case GL_BOOL_VEC2:
		RowCount = 1; ColumnCount = 2; return;

	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
	case GL_UNSIGNED_INT_VEC3:
	case GL_DOUBLE_VEC3:
	case GL_BOOL_VEC3:
		RowCount = 1; ColumnCount = 3; return;

	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT_VEC4:
	case GL_DOUBLE_VEC4:
	case GL_BOOL_VEC4:
		RowCount = 1; ColumnCount = 4; return;

	case GL_FLOAT_MAT2:
	case GL_DOUBLE_MAT2:
		RowCount = 2; ColumnCount = 2; return;

	case GL_FLOAT_MAT3:
	case GL_DOUBLE_MAT3:
		RowCount = 3; ColumnCount = 3; return;

	case GL_FLOAT_MAT4:
	case GL_DOUBLE_MAT4:
		RowCount = 4; ColumnCount = 4; return;

	case GL_FLOAT_MAT2x3:
	case GL_DOUBLE_MAT2x3:
		RowCount = 2; ColumnCount = 3; return;

	case GL_FLOAT_MAT3x2:
	case GL_DOUBLE_MAT3x2:
		RowCount = 3; ColumnCount = 2; return;

	case GL_FLOAT_MAT2x4:
	case GL_DOUBLE_MAT2x4:
		RowCount = 2; ColumnCount = 4; return;

	case GL_FLOAT_MAT4x2:
	case GL_DOUBLE_MAT4x2:
		RowCount = 4; ColumnCount = 2; return;

	case GL_FLOAT_MAT3x4:
	case GL_DOUBLE_MAT3x4:
		RowCount = 3; ColumnCount = 4; return;

	case GL_FLOAT_MAT4x3: 
	case GL_DOUBLE_MAT4x3:
		RowCount = 4; ColumnCount = 3; return;
	}

#ifdef DEBUG
	Log::Stream(LoggingLevel::Internal)
		<< "Failed to find uniform type '"  << uniformType << "'.";
#endif
}
//-----------------------------------------------------------------------
static
EffectAnnotation ToEffectAnnotation(UniformVariableGL4 const& uniform)
{
	EffectAnnotation annotation;
	
	annotation.ParameterType = ToEffectParameterType(uniform.Type);
	annotation.ParameterClass = ToEffectParameterClass(uniform.Type);
	ToComponents(uniform.Type, annotation.RowCount, annotation.ColumnCount);
	
	return std::move(annotation);
}
//-----------------------------------------------------------------------
static
std::vector<EffectVariableDescription> GetEffectVariables(std::vector<UniformVariableGL4> const& uniforms)
{
	std::vector<EffectVariableDescription> result;
	result.reserve(uniforms.size());
	
	for (auto & uniform: uniforms)
	{
		EffectVariableDescription effectVariable;
		effectVariable.Name = uniform.Name;
		effectVariable.ByteSize = uniform.ByteLength;
		effectVariable.Annotation = ToEffectAnnotation(uniform);
	}
	return std::move(result);
}
//-----------------------------------------------------------------------
#ifdef DEBUG
static
void DebugLogUniformBlocks(std::vector<UniformBlockGL4> const& uniformBlocks)
{
	auto stream = Log::Stream(LoggingLevel::Internal);
	
	for (auto const& uniformBlock: uniformBlocks)
	{
		stream
		<< "-[UniformBlock]-------------------\n"
		<< "         Name: " << uniformBlock.Name << "\n"
		<< "   BlockIndex: " << uniformBlock.BlockIndex << "\n"
		<< "ByteConstants: " << uniformBlock.ByteConstants << "\n"
		<< "Uniforms.size: " << uniformBlock.Uniforms.size() << "\n";

		for (auto const& uniform: uniformBlock.Uniforms) {
			stream
			<< ":- - - - - - - - - - - - - -\n"
			<< "         Name: " << uniform.Name << "\n"
			<< "       Offset: " << uniform.Offset << "\n"
			<< "   ByteLength: " << uniform.ByteLength << "\n"
			<< "         Type: " << uniform.Type << "\n"
			<< "  ArrayStride: " << uniform.ArrayStride << "\n"
			<< " MatrixStride: " << uniform.MatrixStride << "\n"
			<< "   IsRowMajor: " << (uniform.IsRowMajor? "true": "false") << "\n";
		}
	}
	
	stream << "--------------------\n";
}
#endif
//-----------------------------------------------------------------------

}// unnamed namespace
//-----------------------------------------------------------------------
ShaderReflectionGL4::ShaderReflectionGL4(ShaderProgramGL4 const& shaderProgramIn)
	: shaderProgram(shaderProgramIn)
{
}
//-----------------------------------------------------------------------
std::vector<UniformBlockGL4> ShaderReflectionGL4::GetNativeUniformBlocks()
{
	auto uniformBlocks = EnumerateUniformBlocks(shaderProgram);
	
	#ifdef DEBUG
	DebugLogUniformBlocks(uniformBlocks);
	#endif
	
	return std::move(uniformBlocks);
}
//-----------------------------------------------------------------------
std::vector<EffectBufferDescription> ShaderReflectionGL4::GetConstantBuffers() const
{
	auto uniformBlocks = EnumerateUniformBlocks(shaderProgram);
	
	std::vector<EffectBufferDescription> result;
	
	for (auto & uniformBlock: uniformBlocks)
	{
		EffectBufferDescription description;
		description.Name = uniformBlock.Name;
		description.ByteConstants = uniformBlock.ByteConstants;
		description.Variables = GetEffectVariables(uniformBlock.Uniforms);
		result.push_back(std::move(description));
	}
	
	return std::move(result);
}

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
