// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_CONTROL_50F0BA0B_HPP
#define POMDOG_CONTROL_50F0BA0B_HPP

#include "UIView.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class Control: public UIView {
public:
    virtual ~Control() = default;

    Control(Matrix3x2 const& transformIn, std::uint32_t widthIn, std::uint32_t heightIn)
        : UIView(transformIn, widthIn, heightIn)
    {}

//    UI::HorizontalAlignment HorizontalAlignment() const final override { return horizontalAlignment; }
//    UI::VerticalAlignment VerticalAlignment() const final override { return verticalAlignment; }
//
//    void HorizontalAlignment(UI::HorizontalAlignment alighnment) { this->horizontalAlignment = alighnment; }
//    void VerticalAlignment(UI::VerticalAlignment alighnment) { this->verticalAlignment = alighnment; }
//
//private:
//    UI::HorizontalAlignment horizontalAlignment = UI::HorizontalAlignment::Stretch;
//    UI::VerticalAlignment verticalAlignment = UI::VerticalAlignment::Stretch;
};

}// namespace UI
}// namespace Pomdog

#endif // POMDOG_CONTROL_50F0BA0B_HPP
