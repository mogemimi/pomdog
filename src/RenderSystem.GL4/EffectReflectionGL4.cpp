//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "EffectReflectionGL4.hpp"
#include "ErrorChecker.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Logging/LogLevel.hpp"
#include "Pomdog/Logging/LogStream.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>
#include <algorithm>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
namespace {
//-----------------------------------------------------------------------
static GLint GetActiveUniformBlockIntValue(ShaderProgramGL4 const& shaderProgram,
	GLuint uniformBlockIndex, GLenum parameterName)
{
	GLint result = 0;
	glGetActiveUniformBlockiv(shaderProgram.value, uniformBlockIndex, parameterName, &result);

	#ifdef DEBUG
	ErrorChecker::CheckError("glGetActiveUniformBlockiv", __FILE__, __LINE__);
	#endif

	return std::move(result);
}
//-----------------------------------------------------------------------
static std::string GetActiveUniformBlockName(
	ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex)
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
static std::vector<GLuint> GetActiveUniformBlockIndices(
	ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex)
{
	auto const uniformCount = GetActiveUniformBlockIntValue(
		shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS);

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
static void GetActiveUniformsIntValue(ShaderProgramGL4 const& shaderProgram,
	std::vector<GLuint> const& uniformIndices, GLenum parameter, Func func)
{
	auto const uniformCount = static_cast<GLsizei>(uniformIndices.size());

	std::vector<GLint> values(uniformIndices.size());

	glGetActiveUniformsiv(shaderProgram.value, uniformCount,
		uniformIndices.data(), parameter, values.data());

	for (GLsizei index = 0; index < uniformCount; ++index)
	{
		func(index, values[index]);
	}
}
//-----------------------------------------------------------------------
static std::vector<UniformVariableGL4> EnumerateUniformVariables(
	ShaderProgramGL4 const& shaderProgram, GLuint uniformBlockIndex)
{
	auto const uniformIndices = GetActiveUniformBlockIndices(shaderProgram, uniformBlockIndex);

	std::vector<UniformVariableGL4> uniforms(uniformIndices.size());

	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_OFFSET,
		[&](GLsizei index, GLint value){ uniforms[index].StartOffset = value; });

	GetActiveUniformsIntValue(shaderProgram, uniformIndices, GL_UNIFORM_SIZE,
		[&](GLsizei index, GLint value){ uniforms[index].Elements = value; });

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
		///@note This shader has no uniform-block.
		return {};
	}

	std::vector<UniformBlockGL4> uniformBlocks;

	for (GLint index = 0; index < uniformBlockCount; ++index)
	{
		auto const uniformBlockIndex = static_cast<GLuint>(index);

		UniformBlockGL4 uniformBlock;

		uniformBlock.Name = GetActiveUniformBlockName(shaderProgram, uniformBlockIndex);
		uniformBlock.ByteSize = GetActiveUniformBlockIntValue(
			shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE);
		//uniformBlock.BindingPoint = GetActiveUniformBlockIntValue(shaderProgram, uniformBlockIndex, GL_UNIFORM_BLOCK_BINDING);
		uniformBlock.Uniforms = EnumerateUniformVariables(shaderProgram, uniformBlockIndex);
		//uniformBlock.BlockIndex = uniformBlockIndex;
		uniformBlock.BlockIndex = glGetUniformBlockIndex(shaderProgram.value, uniformBlock.Name.data());

		uniformBlocks.push_back(std::move(uniformBlock));
	}

	return std::move(uniformBlocks);
}
//-----------------------------------------------------------------------
static std::vector<UniformGL4> EnumerateUniforms(ShaderProgramGL4 const& shaderProgram)
{
	GLint uniformCount = 0;
	glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORMS, &uniformCount);

	#ifdef DEBUG
	ErrorChecker::CheckError("glGetProgramiv", __FILE__, __LINE__);
	#endif

	if (uniformCount <= 0)
	{
		///@note This shader has no uniform.
		return {};
	}

	GLint maxUniformNameLength = 0;
	glGetProgramiv(shaderProgram.value, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

	#ifdef DEBUG
	ErrorChecker::CheckError("glGetProgramiv", __FILE__, __LINE__);
	#endif

	if (maxUniformNameLength <= 0)
	{
		return {};
	}

	maxUniformNameLength += 1;
	std::vector<GLchar> name(maxUniformNameLength, '\0');

	std::vector<UniformGL4> uniformVariables;

	for (GLuint uniformIndex = 0; uniformIndex < static_cast<GLuint>(uniformCount); ++uniformIndex)
	{
		GLint arrayCount = 0;
		GLenum uniformType = 0;
		GLint uniformNameLength = 0;

		glGetActiveUniform(shaderProgram.value, uniformIndex, maxUniformNameLength,
			&uniformNameLength, &arrayCount, &uniformType, name.data());

		#ifdef DEBUG
		ErrorChecker::CheckError("glGetActiveUniform", __FILE__, __LINE__);
		#endif

		POMDOG_ASSERT(uniformNameLength > 0);

		auto const location = glGetUniformLocation(shaderProgram.value, name.data());

		#ifdef DEBUG
		ErrorChecker::CheckError("glGetUniformLocation", __FILE__, __LINE__);
		#endif

		if (location != -1) {
			///@note When uniform location is '-1', it is uniform variable in uniform block.
			UniformGL4 uniform;
			uniform.Name.assign(name.data(), uniformNameLength);
			uniform.Type = uniformType;
			uniform.ArrayCount = arrayCount;
			uniform.Location = location;

			POMDOG_ASSERT(uniform.Type == GL_SAMPLER_1D
				|| uniform.Type == GL_SAMPLER_2D
				|| uniform.Type == GL_SAMPLER_3D
				|| uniform.Type == GL_SAMPLER_CUBE
				|| uniform.Type == GL_SAMPLER_1D_SHADOW
				|| uniform.Type == GL_SAMPLER_2D_SHADOW
				|| uniform.Type == GL_SAMPLER_1D_ARRAY
				|| uniform.Type == GL_SAMPLER_2D_ARRAY
				|| uniform.Type == GL_SAMPLER_1D_ARRAY_SHADOW
				|| uniform.Type == GL_SAMPLER_2D_ARRAY_SHADOW
				|| uniform.Type == GL_SAMPLER_2D_MULTISAMPLE
				|| uniform.Type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY
				|| uniform.Type == GL_SAMPLER_CUBE_SHADOW
				|| uniform.Type == GL_SAMPLER_BUFFER
				|| uniform.Type == GL_SAMPLER_2D_RECT
				|| uniform.Type == GL_SAMPLER_2D_RECT_SHADOW
				|| uniform.Type == GL_INT_SAMPLER_1D
				|| uniform.Type == GL_INT_SAMPLER_2D
				|| uniform.Type == GL_INT_SAMPLER_3D
				|| uniform.Type == GL_INT_SAMPLER_CUBE
				|| uniform.Type == GL_INT_SAMPLER_1D_ARRAY
				|| uniform.Type == GL_INT_SAMPLER_2D_ARRAY
				|| uniform.Type == GL_INT_SAMPLER_2D_MULTISAMPLE
				|| uniform.Type == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
				|| uniform.Type == GL_INT_SAMPLER_BUFFER
				|| uniform.Type == GL_INT_SAMPLER_2D_RECT
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_1D
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_3D
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_CUBE
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_BUFFER
				|| uniform.Type == GL_UNSIGNED_INT_SAMPLER_2D_RECT);

			uniformVariables.push_back(std::move(uniform));
		}
	}

	///@todo Replace the following code by GL_ARB_shading_language_420pack on OpenGL 4.2+:
	std::sort(std::begin(uniformVariables), std::end(uniformVariables),
		[](UniformGL4 const& a, UniformGL4 const& b){ return a.Name < b.Name; });

	return std::move(uniformVariables);
}
//-----------------------------------------------------------------------
static EffectVariableType ToEffectVariableType(GLenum uniformType)
{
	switch (uniformType) {
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
		return EffectVariableType::Float;

	case GL_INT: 
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		return EffectVariableType::Int32;

	case GL_UNSIGNED_INT:
	case GL_UNSIGNED_INT_VEC2:
	case GL_UNSIGNED_INT_VEC3:
	case GL_UNSIGNED_INT_VEC4:
		return EffectVariableType::UInt32;

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
		return EffectVariableType::Double;

	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		return EffectVariableType::Bool;

	case GL_SAMPLER_1D:
		return EffectVariableType::Texture1D;
	case GL_SAMPLER_2D:
		return EffectVariableType::Texture2D;
	case GL_SAMPLER_3D:
		return EffectVariableType::Texture3D;
	case GL_SAMPLER_CUBE:
		return EffectVariableType::TextureCube;

	case GL_SAMPLER_BUFFER:
		//return EffectVariableType::TextureBuffer; // See also: D3D_SVT_TBUFFER
	case GL_SAMPLER_1D_ARRAY:
		//return EffectVariableType::Texture1DArray; // See also: D3D_SVT_TEXTURE1DARRAY
	case GL_SAMPLER_2D_ARRAY:
		//return EffectVariableType::Texture2DArray; // See also: D3D_SVT_TEXTURE2DARRAY
	case GL_SAMPLER_2D_MULTISAMPLE:
		//return EffectVariableType::Texture2DMultiSample; // See also: D3D_SVT_TEXTURE2DMS
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		//return EffectVariableType::Texture2DMultiSampleArray; // See also: D3D_SVT_TEXTURE2DMSARRAY
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
	Log::Stream("Pomdog.RenderSystem", LogLevel::Internal)
		<< "Failed to find effect parameter type '"  << uniformType << "'.";
#endif
	return EffectVariableType::Float;
}
//-----------------------------------------------------------------------
static EffectVariableClass ToEffectVariableClass(GLenum uniformType)
{
	switch (uniformType) {
	case GL_FLOAT:
	case GL_DOUBLE:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_BOOL:
		return EffectVariableClass::Scalar;

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
		return EffectVariableClass::Vector;

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
		return EffectVariableClass::Matrix;

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
		return EffectVariableClass::Object;
	}

#ifdef DEBUG
	Log::Stream("Pomdog.RenderSystem", LogLevel::Internal)
		<< "Failed to find effect parameter class '"  << uniformType << "'.";
#endif
	return EffectVariableClass::Struct;
}
//-----------------------------------------------------------------------
static void ToComponents(GLenum uniformType, std::uint8_t & RowCount, std::uint8_t & ColumnCount)
{
	switch (uniformType) {
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
	Log::Stream("Pomdog.RenderSystem", LogLevel::Internal)
		<< "Failed to find uniform type '"  << uniformType << "'.";
#endif
}
//-----------------------------------------------------------------------
static EffectAnnotation ToEffectAnnotation(UniformVariableGL4 const& uniform)
{
	EffectAnnotation annotation;

	annotation.VariableType = ToEffectVariableType(uniform.Type);
	annotation.VariableClass = ToEffectVariableClass(uniform.Type);
	ToComponents(uniform.Type, annotation.RowCount, annotation.ColumnCount);
	annotation.Elements = (uniform.Elements > 1) ? uniform.Elements: 0;
	POMDOG_ASSERT(annotation.Elements != 1);
	return std::move(annotation);
}
//-----------------------------------------------------------------------
static std::vector<EffectVariable> GetEffectVariables(std::vector<UniformVariableGL4> const& uniforms)
{
	std::vector<EffectVariable> result;
	result.reserve(uniforms.size());

	for (auto & uniform: uniforms)
	{
		EffectVariable effectVariable;
		effectVariable.Name = uniform.Name;
		effectVariable.StartOffset = uniform.StartOffset;
		effectVariable.Annotation = ToEffectAnnotation(uniform);
		result.push_back(std::move(effectVariable));
	}
	return std::move(result);
}
//-----------------------------------------------------------------------
#if defined(DEBUG) && !defined(NDEBUG)
static void DebugLogUniformBlocks(std::vector<UniformBlockGL4> const& uniformBlocks)
{
	auto stream = Log::Stream("Pomdog.RenderSystem", LogLevel::Internal);

	for (auto const& uniformBlock: uniformBlocks)
	{
		stream
		<< "-[UniformBlock]-------------------\n"
		<< "         Name: " << uniformBlock.Name << "\n"
		<< "   BlockIndex: " << uniformBlock.BlockIndex << "\n"
		<< "     ByteSize: " << uniformBlock.ByteSize << "\n"
		<< "Uniforms.size: " << uniformBlock.Uniforms.size() << "\n";

		for (auto const& uniform: uniformBlock.Uniforms) {
			stream
			<< ":- - - - - - - - - - - - - -\n"
			<< "         Name: " << uniform.Name << "\n"
			<< "  StartOffset: " << uniform.StartOffset << "\n"
			<< "     Elements: " << uniform.Elements << "\n"
			<< "         Type: " << uniform.Type << "\n"
			<< "  ArrayStride: " << uniform.ArrayStride << "\n"
			<< " MatrixStride: " << uniform.MatrixStride << "\n"
			<< "   IsRowMajor: " << (uniform.IsRowMajor? "true": "false") << "\n";
		}
	}

	stream << "--------------------\n";
}
//-----------------------------------------------------------------------
static void DebugLogUniforms(std::vector<UniformGL4> const& uniforms)
{
	auto stream = Log::Stream("Pomdog.RenderSystem", LogLevel::Internal);

	for (auto const& uniform: uniforms)
	{
		stream
		<< "-[Uniform]-------------------\n"
		<< "      Name: " << uniform.Name << "\n"
		<< "  Location: " << uniform.Location << "\n"
		<< "      Type: " << uniform.Type << "\n"
		<< "ArrayCount: " << uniform.ArrayCount << "\n";
	}

	stream << "--------------------\n";
}
#endif

}// unnamed namespace
//-----------------------------------------------------------------------
EffectReflectionGL4::EffectReflectionGL4(ShaderProgramGL4 const& shaderProgramIn)
	: shaderProgram(shaderProgramIn)
{
}
//-----------------------------------------------------------------------
std::vector<UniformBlockGL4> EffectReflectionGL4::GetNativeUniformBlocks()
{
	auto uniformBlocks = EnumerateUniformBlocks(shaderProgram);

	#ifdef DEBUG
	DebugLogUniformBlocks(uniformBlocks);
	#endif

	return std::move(uniformBlocks);
}
//-----------------------------------------------------------------------
std::vector<UniformGL4> EffectReflectionGL4::GetNativeUniforms()
{
	auto uniforms = EnumerateUniforms(shaderProgram);

	#ifdef DEBUG
	DebugLogUniforms(uniforms);
	#endif

	return std::move(uniforms);
}
//-----------------------------------------------------------------------
std::vector<EffectConstantDescription> EffectReflectionGL4::GetConstantBuffers() const
{
	auto uniformBlocks = EnumerateUniformBlocks(shaderProgram);

	std::vector<EffectConstantDescription> result;

	for (auto & uniformBlock: uniformBlocks)
	{
		EffectConstantDescription description;
		description.Name = uniformBlock.Name;
		description.ByteSize = uniformBlock.ByteSize;
		description.Variables = GetEffectVariables(uniformBlock.Uniforms);
		result.push_back(std::move(description));
	}

	return std::move(result);
}

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
