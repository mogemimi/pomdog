//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import <Cocoa/Cocoa.h>

#include <Pomdog/Application/Bootstrapper.hpp>
#include "CocoaTestGame.hpp"

int main(int argc, const char * argv[])
{
	Pomdog::Bootstrapper<Pomdog::CocoaTestGame> bootstrapper;
	return NSApplicationMain(argc, argv);
}
