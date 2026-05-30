import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'secrets.dart';  // TTN API Key — nicht im Git!

// ── Konfiguration ───────────────────────────────────────────
const String firebaseDbUrl  = 'https://smartgarden-app-ollie-default-rtdb.europe-west1.firebasedatabase.app';
const String ttnAppId       = 'smartgardenollie';
const String ttnDeviceId    = 'ollie-smartgarden-device';
const String ttnMqttHost    = 'eu1.cloud.thethings.network';

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
  bool   _ledState    = false;
  bool   _motionAlert = false;
  bool   _connected   = false;
  String _lastUpdate  = '—';
  Timer? _timer;

  @override
  void initState() {
    super.initState();
    _fetchStatus();
    // alle 10s aktualisieren
    _timer = Timer.periodic(const Duration(seconds: 10), (_) => _fetchStatus());
  }

  Future<void> _fetchStatus() async {
    final url = Uri.parse(
      '$firebaseDbUrl/devices/$ttnDeviceId.json'
    );
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        if (data == null) return;

        // TTN schreibt mit Push-Key — letzten Eintrag finden
        final entries = (data as Map).values.toList();
        if (entries.isEmpty) return;

        // Letzten Uplink nehmen
        final last = entries.last;
        final frmPayload = last['uplink_message']?['frm_payload'] as String?;
        if (frmPayload == null) return;

        final bytes  = base64Decode(frmPayload);
        final led    = bytes.isNotEmpty ? bytes[0] == 0x01 : false;
        final motion = bytes.length > 1 ? bytes[1] == 0x01 : false;
        final time   = last['received_at'] as String? ?? '—';

        setState(() {
          _ledState    = led;
          _motionAlert = motion;
          _connected   = true;
          _lastUpdate  = time.length > 18 ? time.substring(11, 19) : time;
        });
      }
    } catch (e) {
      debugPrint('Fetch-Fehler: $e');
      setState(() => _connected = false);
    }
  }

  Future<void> _sendDownlink(int cmd) async {
    final url = Uri.parse(
      'https://$ttnMqttHost/api/v3/as/applications/$ttnAppId/devices/$ttnDeviceId/down/push'
    );
    final payload = jsonEncode({
      'downlinks': [
        {
          'frm_payload': base64Encode([cmd]),
          'f_port': 1,
          'priority': 'NORMAL',
        }
      ]
    });

    try {
      final response = await http.post(
        url,
        headers: {
          'Authorization': 'Bearer $ttnApiKey',
          'Content-Type': 'application/json',
        },
        body: payload,
      );
      debugPrint('[DOWN] Status: ${response.statusCode}');
    } catch (e) {
      debugPrint('[DOWN] Fehler: $e');
    }
  }

  @override
  void dispose() {
    _timer?.cancel();
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

            const SizedBox(height: 8),
            TextButton.icon(
              onPressed: _fetchStatus,
              icon: const Icon(Icons.refresh),
              label: const Text('Aktualisieren'),
            ),

            const SizedBox(height: 32),
            const Text('LED steuern',
                style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold)),
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
