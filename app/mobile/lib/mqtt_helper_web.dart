import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_browser_client.dart';

MqttClient createMqttClient(String host, String clientId, int port) {
  final client = MqttBrowserClient('wss://eu1.cloud.thethings.network:8884', clientId);
  return client;
}
