// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PANEL_C95AA33F_HPP
#define POMDOG_PANEL_C95AA33F_HPP

#include "UIView.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace UI {

class Panel: public UIView {
public:
    virtual ~Panel() = default;

    Panel(Matrix3x2 const& transformIn, std::uint32_t widthIn, std::uint32_t heightIn)
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

#endif // POMDOG_PANEL_C95AA33F_HPP
