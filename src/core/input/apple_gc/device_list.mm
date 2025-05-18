#include "core/input/apple_gc/device_list.hpp"

#import <GameController/GameController.h>

#include "core/input/apple_gc/keyboard.hpp"

using DeviceList = hydra::input::apple_gc::DeviceList;

@interface DeviceListImpl : NSObject

@property DeviceList* parent;

@end

@implementation DeviceListImpl

- (id)initWithParent:(DeviceList*)parent {
    if (self = [super init]) {
        self.parent = parent;

        [[NSNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(controllerConnected:)
                   name:GCControllerDidConnectNotification
                 object:nil];
        [[NSNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(controllerDisconnected:)
                   name:GCControllerDidDisconnectNotification
                 object:nil];
        [[NSNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(keyboardConnected:)
                   name:GCKeyboardDidConnectNotification
                 object:nil];
        [[NSNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(keyboardDisconnected:)
                   name:GCKeyboardDidDisconnectNotification
                 object:nil];
    }

    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)controllerConnected:(NSNotification*)notification {
    GCController* controller = (GCController*)notification.object;
    _parent->_AddController(controller);
}

- (void)controllerDisconnected:(NSNotification*)notification {
    GCController* controller = (GCController*)notification.object;
    _parent->_RemoveController(controller);
}

- (void)keyboardConnected:(NSNotification*)notification {
    GCKeyboard* keyboard = (GCKeyboard*)notification.object;
    _parent->_AddKeyboard(keyboard);
}

- (void)keyboardDisconnected:(NSNotification*)notification {
    GCKeyboard* keyboard = (GCKeyboard*)notification.object;
    _parent->_RemoveKeyboard(keyboard);
}

@end

namespace hydra::input::apple_gc {

DeviceList::DeviceList() {
    impl = [[DeviceListImpl alloc] initWithParent:this];
}

DeviceList::~DeviceList() { [impl release]; }

void DeviceList::_AddController(id controller) {
    LOG_FUNC_NOT_IMPLEMENTED(Input);
}

void DeviceList::_RemoveController(id controller) {
    LOG_FUNC_NOT_IMPLEMENTED(Input);
}

void DeviceList::_AddKeyboard(id keyboard) {
    // TODO: don't hardcode the name
    devices["keyboard"] = new Keyboard(keyboard);
}

void DeviceList::_RemoveKeyboard(id keyboard) {
    // TODO: don't hardcode the name
    devices.erase("keyboard");
}

} // namespace hydra::input::apple_gc
