import 'package:flutter/material.dart';
import 'dart:async';
import 'dart:typed_data';

import 'package:flutter/services.dart';
import 'package:file_icon_and_metadata/file_icon_and_metadata.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  Uint8List? _iconBytes;
  final _fileIconAndMetadataPlugin = FileIconAndMetadata();

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Initialize and load platform/version and icon data.
  Future<void> initPlatformState() async {
    String platformVersion;
    Uint8List? iconBytes;

    // Get platform version safely.
    try {
      platformVersion = await _fileIconAndMetadataPlugin.getPlatformVersion() ??
          'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    // Get file icon data directly as bytes.
    try {
      iconBytes = await _fileIconAndMetadataPlugin.getFileIcon(
          "C:/Users/Administrator/Desktop/Godot_v4.3-stable_win64.exe");
    } on PlatformException {
      // Handle errors if needed.
    }

    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
      _iconBytes = iconBytes;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: _iconBytes != null
              ? Image.memory(_iconBytes!)
              : const Text('Nothing to see here'),
        ),
      ),
    );
  }
}
