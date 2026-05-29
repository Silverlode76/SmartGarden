import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'secrets.dart';  // API Key — nicht im Git!

// ── TTN Konfiguration ───────────────────────────────────────
const String ttnAppId    = 'smartgardenollie';
const String ttnDeviceId = 'ollie-smartgarden-device';
const String ttnMqttHost = 'eu1.cloud.thethings.network';
const int    ttnMqttPort = 8883;

void main() {
  runApp(const SmartGardenApp());
}

class SmartGardenApp extends StatelessWidget {
  const SmartGardenApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'SmartGarden',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.green),
        useMaterial3: true,
      ),
      home: const SmartGardenHome(),
    );
  }
}

class SmartGardenHome extends StatefulWidget {
  const SmartGardenHome({super.key});

  @override
  State<SmartGardenHome> createState() => _SmartGardenHomeState();
}

class _SmartGardenHomeState extends State<SmartGardenHome> {
  MqttServerClient? _client;

  bool   _ledState    = false;
  bool   _motionAlert = false;
  bool   _connected   = false;
  String _lastUpdate  = '—';

  @override
  void initState() {
    super.initState();
    _connectMqtt();
  }

  Future<void> _connectMqtt() async {
    final client = MqttServerClient.withPort(
      ttnMqttHost,
      'flutter_sg_${DateTime.now().millisecondsSinceEpoch}',
      ttnMqttPort,
    );
    client.secure = true;
    client.keepAlivePeriod = 60;
    client.onDisconnected = _onDisconnected;

    final connMsg = MqttConnectMessage()
        .withClientIdentifier('flutter_sg')
        .authenticateAs('$ttnAppId@ttn', ttnApiKey)
        .startClean();
    client.connectionMessage = connMsg;

    try {
      await client.connect();
    } catch (e) {
      debugPrint('MQTT Verbindungsfehler: $e');
      return;
    }

    if (client.connectionStatus?.state == MqttConnectionState.connected) {
      setState(() {
        _connected = true;
        _client = client;
      });

      final topic = 'v3/$ttnAppId@ttn/devices/$ttnDeviceId/up';
      client.subscribe(topic, MqttQos.atLeastOnce);

      client.updates?.listen((List<MqttReceivedMessage<MqttMessage>> messages) {
        final msg = messages[0].payload as MqttPublishMessage;
        final payload = MqttPublishPayload.bytesToStringAsString(msg.payload.message);
        _parseUplink(payload);
      });
    }
  }

  void _parseUplink(String raw) {
    try {
      final json = jsonDecode(raw);
      final frmPayload = json['uplink_message']?['frm_payload'] as String?;
      if (frmPayload == null) return;

      final bytes  = base64Decode(frmPayload);
      final led    = bytes.isNotEmpty ? bytes[0] == 0x01 : false;
      final motion = bytes.length > 1 ? bytes[1] == 0x01 : false;

      setState(() {
        _ledState    = led;
        _motionAlert = motion;
        _lastUpdate  = TimeOfDay.now().format(context);
      });
    } catch (e) {
      debugPrint('Parse-Fehler: $e');
    }
  }

  void _sendDownlink(int cmd) {
    if (_client == null || !_connected) return;

    final topic = 'v3/$ttnAppId@ttn/devices/$ttnDeviceId/down/push';
    final payload = jsonEncode({
      'downlinks': [
        {
          'frm_payload': base64Encode([cmd]),
          'f_port': 1,
          'priority': 'NORMAL',
        }
      ]
    });

    final builder = MqttClientPayloadBuilder();
    builder.addString(payload);
    _client!.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);
    debugPrint('[MQTT] Downlink: 0x${cmd.toRadixString(16)}');
  }

  void _onDisconnected() {
    setState(() => _connected = false);
  }

  @override
  void dispose() {
    _client?.disconnect();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('SmartGarden'),
        backgroundColor: Colors.green,
        foregroundColor: Colors.white,
        actions: [
          Padding(
            padding: const EdgeInsets.all(12),
            child: Icon(
              _connected ? Icons.wifi : Icons.wifi_off,
              color: _connected ? Colors.white : Colors.red[200],
            ),
          ),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(24),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            // Status-Karten
            Row(
              children: [
                Expanded(child: _StatusCard(
                  icon: Icons.lightbulb,
                  label: 'LED',
                  value: _ledState ? 'AN' : 'AUS',
                  color: _ledState ? Colors.amber : Colors.grey,
                )),
                const SizedBox(width: 16),
                Expanded(child: _StatusCard(
                  icon: Icons.directions_run,
                  label: 'Bewegung',
                  value: _motionAlert ? 'ALARM' : 'KLAR',
                  color: _motionAlert ? Colors.red : Colors.green,
                )),
              ],
            ),

            const SizedBox(height: 8),
            Text(
              'Letztes Update: $_lastUpdate',
              textAlign: TextAlign.center,
              style: const TextStyle(color: Colors.grey, fontSize: 12),
            ),

            const SizedBox(height: 40),
            const Text(
              'LED steuern',
              style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 12),

            Row(
              children: [
                Expanded(child: ElevatedButton.icon(
                  onPressed: () => _sendDownlink(0x01),
                  icon: const Icon(Icons.lightbulb),
                  label: const Text('AN'),
                  style: ElevatedButton.styleFrom(backgroundColor: Colors.amber),
                )),
                const SizedBox(width: 12),
                Expanded(child: ElevatedButton.icon(
                  onPressed: () => _sendDownlink(0x00),
                  icon: const Icon(Icons.lightbulb_outline),
                  label: const Text('AUS'),
                  style: ElevatedButton.styleFrom(backgroundColor: Colors.grey[300]),
                )),
                const SizedBox(width: 12),
                Expanded(child: ElevatedButton.icon(
                  onPressed: () => _sendDownlink(0x02),
                  icon: const Icon(Icons.swap_horiz),
                  label: const Text('Toggle'),
                  style: ElevatedButton.styleFrom(backgroundColor: Colors.blue[100]),
                )),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

class _StatusCard extends StatelessWidget {
  final IconData icon;
  final String   label;
  final String   value;
  final Color    color;

  const _StatusCard({
    required this.icon,
    required this.label,
    required this.value,
    required this.color,
  });

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            Icon(icon, size: 40, color: color),
            const SizedBox(height: 8),
            Text(label, style: const TextStyle(fontSize: 12, color: Colors.grey)),
            Text(value, style: TextStyle(
              fontSize: 20,
              fontWeight: FontWeight.bold,
              color: color,
            )),
          ],
        ),
      ),
    );
  }
}
