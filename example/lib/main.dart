import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:flutter_ar_vr/flutter_ar_vr.dart';

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
  String _vrStatus = 'Not Initialized';
  final _flutterArVrPlugin = FlutterArVr();

  @override
  void initState() {
    super.initState();
    initPlatformState();
    initializeVr(); // Call the method to initialize VR
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    try {
      platformVersion = await _flutterArVrPlugin.getPlatformVersion() ??
          'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
    });
  }

  // Method to initialize VR
  Future<void> initializeVr() async {
    try {
      String vrStatus =
          await _flutterArVrPlugin.initialize() ?? 'Unknown VR status';
      if (!mounted) return;

      setState(() {
        _vrStatus = vrStatus;
      });
    } on PlatformException catch (e) {
      setState(() {
        _vrStatus = 'Failed to initialize VR: ${e.message}';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text('Running on: $_platformVersion\n'),
              Text('VR Status: $_vrStatus\n'),
            ],
          ),
        ),
      ),
    );
  }
}
