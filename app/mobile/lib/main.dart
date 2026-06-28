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

// Irrigator-Node (Bewässerung)
const String ttnIrrigatorAppId    = 'smartgarden-irrigator';
const String ttnIrrigatorDeviceId = 'smartgarden-irrigator';

// AWS-Backend (Lernpfad, parallel zu Firebase) — API Gateway Invoke URL
const String awsApiBaseUrl = 'https://e3trf6ld2j.execute-api.eu-central-1.amazonaws.com';

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

  bool   _pumpOn              = false;
  int    _soilRaw             = 0;
  bool   _irrigatorConnected  = false;
  String _irrigatorLastUpdate = '—';

  bool   _pumpOnAws       = false;
  int    _soilRawAws      = 0;
  bool   _awsConnected    = false;
  String _awsLastUpdate   = '—';

  Timer? _timer;

  @override
  void initState() {
    super.initState();
    _fetchStatus();
    _fetchIrrigatorStatus();
    _fetchIrrigatorStatusAws();
    // alle 10s aktualisieren
    _timer = Timer.periodic(const Duration(seconds: 10), (_) {
      _fetchStatus();
      _fetchIrrigatorStatus();
      _fetchIrrigatorStatusAws();
    });
  }

  // Liest den Irrigator-Status über den AWS-Pfad (API Gateway → Lambda → DynamoDB)
  // — paralleler Lernpfad zu Firebase, gleiche Payload-Struktur (Byte 0 = Pumpe,
  // Byte 1-2 = Bodenfeuchte), nur als Base64 über JSON statt direkt aus TTN.
  Future<void> _fetchIrrigatorStatusAws() async {
    final url = Uri.parse(
      '$awsApiBaseUrl/status?device_id=$ttnIrrigatorDeviceId'
    );
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body) as Map<String, dynamic>;
        final frmPayload = data['frm_payload'] as String?;
        if (frmPayload == null || frmPayload.isEmpty) return;

        final bytes = base64Decode(frmPayload);
        final pump  = bytes.isNotEmpty ? bytes[0] == 0x01 : false;
        final soil  = bytes.length > 2 ? (bytes[1] << 8) | bytes[2] : 0;
        final time  = data['received_at'] as String? ?? '—';

        setState(() {
          _pumpOnAws     = pump;
          _soilRawAws    = soil;
          _awsConnected  = true;
          _awsLastUpdate = time.length > 18 ? time.substring(11, 19) : time;
        });
      }
    } catch (e) {
      debugPrint('AWS Fetch-Fehler: $e');
      setState(() => _awsConnected = false);
    }
  }

  // Extrahiert den neuesten Eintrag aus einer TTN-Webhook-Firebase-Struktur
  // (mehrere Push-Key-Einträge ODER ein einzelnes Objekt)
  Map<String, dynamic>? _latestEntry(dynamic data) {
    if (data == null) return null;
    if (data is Map && data.values.first is Map) {
      return (data as Map).values.cast<Map>().reduce((a, b) {
        final ta = (a['received_at'] ?? '') as String;
        final tb = (b['received_at'] ?? '') as String;
        return ta.compareTo(tb) >= 0 ? a : b;
      }) as Map<String, dynamic>;
    }
    return data as Map<String, dynamic>;
  }

  Future<void> _fetchIrrigatorStatus() async {
    final url = Uri.parse(
      '$firebaseDbUrl/devices/$ttnIrrigatorDeviceId/latest.json'
    );
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        final latest = _latestEntry(jsonDecode(response.body));
        if (latest == null) return;

        final frmPayload = latest['uplink_message']?['frm_payload'] as String?;
        if (frmPayload == null) return;

        final bytes = base64Decode(frmPayload);
        final pump  = bytes.isNotEmpty ? bytes[0] == 0x01 : false;
        final soil  = bytes.length > 2 ? (bytes[1] << 8) | bytes[2] : 0;
        final time  = latest['received_at'] as String? ?? '—';

        setState(() {
          _pumpOn              = pump;
          _soilRaw             = soil;
          _irrigatorConnected  = true;
          _irrigatorLastUpdate = time.length > 18 ? time.substring(11, 19) : time;
        });
      }
    } catch (e) {
      debugPrint('Irrigator Fetch-Fehler: $e');
      setState(() => _irrigatorConnected = false);
    }
  }

  Future<void> _fetchStatus() async {
    final url = Uri.parse(
      '$firebaseDbUrl/devices/$ttnDeviceId/latest.json'
    );
    try {
      final response = await http.get(url);
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        if (data == null) return;

        // Letzten Eintrag nach received_at sortieren
        Map<String, dynamic> latest;
        if (data is Map && data.values.first is Map) {
          // Mehrere Push-Key Einträge — neuesten finden
          latest = (data as Map).values
              .cast<Map>()
              .reduce((a, b) {
                final ta = (a['received_at'] ?? '') as String;
                final tb = (b['received_at'] ?? '') as String;
                return ta.compareTo(tb) >= 0 ? a : b;
              }) as Map<String, dynamic>;
        } else {
          latest = data as Map<String, dynamic>;
        }

        final frmPayload = latest['uplink_message']?['frm_payload'] as String?;
        if (frmPayload == null) return;

        final bytes  = base64Decode(frmPayload);
        final led    = bytes.isNotEmpty ? bytes[0] == 0x01 : false;
        final motion = bytes.length > 1 ? bytes[1] == 0x01 : false;
        final time   = latest['received_at'] as String? ?? '—';

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

            const SizedBox(height: 32),
            const Divider(),
            const SizedBox(height: 16),

            Row(
              children: [
                const Text('Bewässerung',
                    style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold)),
                const Spacer(),
                Icon(
                  _irrigatorConnected ? Icons.wifi : Icons.wifi_off,
                  size: 18,
                  color: _irrigatorConnected ? Colors.green : Colors.red[200],
                ),
              ],
            ),
            const SizedBox(height: 12),

            Row(
              children: [
                Expanded(child: _StatusCard(
                  icon: Icons.water_drop,
                  label: 'Pumpe',
                  value: _pumpOn ? 'AN' : 'AUS',
                  color: _pumpOn ? Colors.blue : Colors.grey,
                )),
                const SizedBox(width: 16),
                Expanded(child: _StatusCard(
                  icon: Icons.grass,
                  label: 'Bodenfeuchte',
                  value: _soilMoisturePercent(),
                  color: _soilMoistureColor(),
                )),
              ],
            ),

            const SizedBox(height: 8),
            Text(
              'Letztes Update: $_irrigatorLastUpdate',
              textAlign: TextAlign.center,
              style: const TextStyle(color: Colors.grey, fontSize: 12),
            ),

            const SizedBox(height: 32),
            const Divider(),
            const SizedBox(height: 16),

            Row(
              children: [
                const Text('Bewässerung (AWS)',
                    style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold)),
                const Spacer(),
                Icon(
                  _awsConnected ? Icons.wifi : Icons.wifi_off,
                  size: 18,
                  color: _awsConnected ? Colors.green : Colors.red[200],
                ),
              ],
            ),
            const SizedBox(height: 12),

            Row(
              children: [
                Expanded(child: _StatusCard(
                  icon: Icons.water_drop,
                  label: 'Pumpe',
                  value: _pumpOnAws ? 'AN' : 'AUS',
                  color: _pumpOnAws ? Colors.blue : Colors.grey,
                )),
                const SizedBox(width: 16),
                Expanded(child: _StatusCard(
                  icon: Icons.grass,
                  label: 'Bodenfeuchte',
                  value: _soilMoisturePercent(_soilRawAws),
                  color: _soilMoistureColor(_soilRawAws),
                )),
              ],
            ),

            const SizedBox(height: 8),
            Text(
              'Letztes Update: $_awsLastUpdate',
              textAlign: TextAlign.center,
              style: const TextStyle(color: Colors.grey, fontSize: 12),
            ),
          ],
        ),
      ),
    );
  }

  // Kalibrierung: trocken ≈ 1671, nass ≈ 667 (siehe firmware/irrigator-node)
  static const int _soilDryRaw = 1671;
  static const int _soilWetRaw = 667;

  String _soilMoisturePercent([int? raw]) {
    final value = raw ?? _soilRaw;
    final pct = ((_soilDryRaw - value) / (_soilDryRaw - _soilWetRaw) * 100)
        .clamp(0, 100)
        .round();
    return '$pct%';
  }

  Color _soilMoistureColor([int? raw]) {
    final value = raw ?? _soilRaw;
    final pct = ((_soilDryRaw - value) / (_soilDryRaw - _soilWetRaw) * 100)
        .clamp(0, 100);
    if (pct < 25) return Colors.red;
    if (pct < 50) return Colors.orange;
    return Colors.green;
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
