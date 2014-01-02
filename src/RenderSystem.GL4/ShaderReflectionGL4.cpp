//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ShaderReflectionGL4.hpp"
#include <utility>
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LoggingLevel.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include "EffectPassGL4.hpp"
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
std::vector<UniformBlockGL4> ShaderReflectionGL4::GetUniformBlocks(EffectPassGL4 & effectPass)
{
	auto shaderProgram = effectPass.GetShaderProgram();

	auto uniformBlocks = EnumerateUniformBlocks(shaderProgram);
	
	#ifdef DEBUG
	DebugLogUniformBlocks(uniformBlocks);
	#endif
	
	return std::move(uniformBlocks);
}

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
