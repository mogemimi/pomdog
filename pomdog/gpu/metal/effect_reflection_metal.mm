// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/effect_reflection_metal.h"
#include "pomdog/gpu/shader_reflections/effect_constant_description.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::metal {
namespace {

[[nodiscard]] EffectVariableType
toEffectVariableType(MTLDataType variableType) noexcept
{
    switch (variableType) {
    case MTLDataTypeFloat:
    case MTLDataTypeFloat2:
    case MTLDataTypeFloat3:
    case MTLDataTypeFloat4:
    case MTLDataTypeFloat2x2:
    case MTLDataTypeFloat2x3:
    case MTLDataTypeFloat2x4:
    case MTLDataTypeFloat3x2:
    case MTLDataTypeFloat3x3:
    case MTLDataTypeFloat3x4:
    case MTLDataTypeFloat4x2:
    case MTLDataTypeFloat4x3:
    case MTLDataTypeFloat4x4:
        return EffectVariableType::Float;
    case MTLDataTypeInt:
    case MTLDataTypeInt2:
    case MTLDataTypeInt3:
    case MTLDataTypeInt4:
        return EffectVariableType::Int32;
    case MTLDataTypeBool:
    case MTLDataTypeBool2:
    case MTLDataTypeBool3:
    case MTLDataTypeBool4:
        return EffectVariableType::Bool;
    case MTLDataTypeNone:
        return EffectVariableType::Void;
    default:
        break;
    }
    return EffectVariableType::Float;
}

[[nodiscard]] EffectVariableClass
toEffectVariableClass(MTLDataType variableClass) noexcept
{
    switch (variableClass) {
    case MTLDataTypeFloat:
    case MTLDataTypeHalf:
    case MTLDataTypeInt:
    case MTLDataTypeUInt:
    case MTLDataTypeShort:
    case MTLDataTypeUShort:
    case MTLDataTypeChar:
    case MTLDataTypeUChar:
    case MTLDataTypeBool:
        return EffectVariableClass::Scalar;
    case MTLDataTypeFloat2:
    case MTLDataTypeFloat3:
    case MTLDataTypeFloat4:
    case MTLDataTypeHalf2:
    case MTLDataTypeHalf3:
    case MTLDataTypeHalf4:
    case MTLDataTypeInt2:
    case MTLDataTypeInt3:
    case MTLDataTypeInt4:
    case MTLDataTypeUInt2:
    case MTLDataTypeUInt3:
    case MTLDataTypeUInt4:
    case MTLDataTypeShort2:
    case MTLDataTypeShort3:
    case MTLDataTypeShort4:
    case MTLDataTypeUShort2:
    case MTLDataTypeUShort3:
    case MTLDataTypeUShort4:
    case MTLDataTypeChar2:
    case MTLDataTypeChar3:
    case MTLDataTypeChar4:
    case MTLDataTypeUChar2:
    case MTLDataTypeUChar3:
    case MTLDataTypeUChar4:
    case MTLDataTypeBool2:
    case MTLDataTypeBool3:
    case MTLDataTypeBool4:
        return EffectVariableClass::Vector;
    case MTLDataTypeFloat2x2:
    case MTLDataTypeFloat2x3:
    case MTLDataTypeFloat2x4:
    case MTLDataTypeFloat3x2:
    case MTLDataTypeFloat3x3:
    case MTLDataTypeFloat3x4:
    case MTLDataTypeFloat4x2:
    case MTLDataTypeFloat4x3:
    case MTLDataTypeFloat4x4:
    case MTLDataTypeHalf2x2:
    case MTLDataTypeHalf2x3:
    case MTLDataTypeHalf2x4:
    case MTLDataTypeHalf3x2:
    case MTLDataTypeHalf3x3:
    case MTLDataTypeHalf3x4:
    case MTLDataTypeHalf4x2:
    case MTLDataTypeHalf4x3:
    case MTLDataTypeHalf4x4:
        return EffectVariableClass::Matrix;
    case MTLDataTypeNone:
    case MTLDataTypeArray:
        return EffectVariableClass::Object;
    case MTLDataTypeStruct:
        return EffectVariableClass::Struct;
    default:
        break;
    }
    return EffectVariableClass::Scalar;
}

} // namespace

EffectReflectionMetal::EffectReflectionMetal(MTLRenderPipelineReflection* reflectionIn)
    : reflection_(reflectionIn)
{
    POMDOG_ASSERT(reflection_ != nullptr);
}

std::vector<EffectConstantDescription>
EffectReflectionMetal::getConstantBuffers() const noexcept
{
    NSArray<MTLArgument*>* arguments = reflection_.vertexArguments;

    std::vector<EffectConstantDescription> constants;

    for (NSUInteger i = 0; i < arguments.count; ++i) {
        MTLArgument* argument = arguments[i];

        if (argument.type != MTLArgumentTypeBuffer) {
            continue;
        }

        EffectConstantDescription constant;

        ///@todo Not implemented
        constant.Name = [argument.name UTF8String];

        POMDOG_ASSERT(argument.bufferDataType == MTLDataTypeStruct);

        MTLStructType* bufferStruct = argument.bufferStructType;
        for (NSUInteger index = 0; index < bufferStruct.members.count; ++index) {
            MTLStructMember* member = bufferStruct.members[index];

            EffectVariable variable;
            variable.Name = [member.name UTF8String];
            variable.StartOffset = static_cast<std::uint32_t>(member.offset);
            variable.Annotation.VariableType = toEffectVariableType(member.dataType);
            variable.Annotation.VariableClass = toEffectVariableClass(member.dataType);

            ///@todo Not implemented
            POMDOG_ASSERT(false);
            variable.Annotation.Elements = 0;
            variable.Annotation.RowCount = 0;
            variable.Annotation.ColumnCount = 0;
            constant.Variables.push_back(std::move(variable));
        }

        ///@todo Not implemented
        constants.push_back(std::move(constant));
        POMDOG_ASSERT(false);
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    ///@todo Not implemented
    //reflection.fragmentArguments;
    POMDOG_ASSERT(false);
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
#else
    return constants;
#endif
}

} // namespace pomdog::gpu::detail::metal
