import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

MqttClient createMqttClient(String host, String clientId, int port) {
  final client = MqttServerClient.withPort(host, clientId, port);
  client.secure = true;
  return client;
}
