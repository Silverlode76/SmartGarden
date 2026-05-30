// Conditional import — automatisch Web oder Native
export 'mqtt_helper_stub.dart'
    if (dart.library.io) 'mqtt_helper_native.dart'
    if (dart.library.html) 'mqtt_helper_web.dart';
