//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_RENDERQUEUE_32005F81_9DE4_4E2F_A57C_BCC9DC5AF832_HPP
#define POMDOG_RENDERQUEUE_32005F81_9DE4_4E2F_A57C_BCC9DC5AF832_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <vector>
#include <memory>
#include <functional>

namespace Pomdog {

class RenderCommand;

class RenderQueue {
public:
	void PushBack(std::reference_wrapper<RenderCommand> && command);

	void Sort();

	std::size_t Count() const;

	void Clear();

	void Enumerate(std::function<void(RenderCommand &)> const& callback) const;

private:
	std::vector<std::reference_wrapper<RenderCommand>> negativeCommands;
	std::vector<std::reference_wrapper<RenderCommand>> positiveCommands;
	std::vector<std::reference_wrapper<RenderCommand>> zeroCommands;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RENDERQUEUE_32005F81_9DE4_4E2F_A57C_BCC9DC5AF832_HPP)
