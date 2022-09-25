import 'dart:async';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:easy_debounce/easy_debounce.dart';
import 'package:safe_eats/models/appliance_model.dart';
import 'dart:convert';

class AppliancesProvider with ChangeNotifier {
  final ValueNotifier<Map<String, String>> _userAppliances = ValueNotifier({});
  final String _uid = FirebaseAuth.instance.currentUser!.uid;
  final Map<String, ApplianceModel> _appliances = {};

  late final StreamSubscription<DatabaseEvent> _userAppliancesAddedSub;
  late final StreamSubscription<DatabaseEvent> _userAppliancesChangedSub;
  late final StreamSubscription<DatabaseEvent> _userAppliancesRemovedSub;
  late List<StreamSubscription<DatabaseEvent>> _appliancesStreams = [];

  late final DatabaseReference _userAppliancesRef = FirebaseDatabase.instance.ref('users/$_uid/appliances');
  late final DatabaseReference _appliancesRef = FirebaseDatabase.instance.ref('appliances');

  Map<String, String> get userAppliances => _userAppliances.value;
  Map<String, ApplianceModel> get appliances => _appliances;

  AppliancesProvider() {
    _listenUserAppliancesStream();
    _listenAppliancesStream();
  }

  void _listenUserAppliancesStream() {
    _userAppliancesAddedSub = _userAppliancesRef.onChildAdded.listen(
      (DatabaseEvent event) async {
        if (event.snapshot.value != null) {
          _userAppliances.value[event.snapshot.key as String] = event.snapshot.value as String;
          _userAppliances.notifyListeners();
          notifyListeners();
        }
      },
      onError: (Object o) {
        final error = o as FirebaseException;
        debugPrint('Error: ${error.code} ${error.message}');
      },
    );

    _userAppliancesChangedSub = _userAppliancesRef.onChildChanged.listen(
      (DatabaseEvent event) {
        if (event.snapshot.value != null) {
          _userAppliances.value[event.snapshot.key as String] = event.snapshot.value as String;
          _userAppliances.notifyListeners();
          notifyListeners();
        }
      },
      onError: (Object o) {
        final error = o as FirebaseException;
        debugPrint('Error: ${error.code} ${error.message}');
      },
    );

    _userAppliancesRemovedSub = _userAppliancesRef.onChildRemoved.listen(
      (DatabaseEvent event) {
        _userAppliances.value.remove(event.snapshot.key as String);
        _userAppliances.notifyListeners();
        notifyListeners();
      },
      onError: (Object o) {
        final error = o as FirebaseException;
        debugPrint('Error: ${error.code} ${error.message}');
      },
    );
  }

  void _listenAppliancesStream() {
    _userAppliances.addListener(
      () {
        EasyDebounce.debounce(
          'firebase-appliances',
          const Duration(milliseconds: 500),
          () {
            for (StreamSubscription<DatabaseEvent> s in _appliancesStreams) {
              s.cancel();
            }
            _appliancesStreams = [];
            _userAppliances.value.forEach(
              (String key, String value) {
                _appliancesStreams.add(
                  _appliancesRef.child(value).onValue.listen(
                    (DatabaseEvent event) {
                      if (event.snapshot.value != null) {
                        _appliances[key] = ApplianceModel.fromJson(jsonDecode(jsonEncode(event.snapshot.value)));
                        notifyListeners();
                      }
                    },
                    onError: (Object o) {
                      final error = o as FirebaseException;
                      debugPrint('Error: ${error.code} ${error.message}');
                    },
                  ),
                );
              },
            );
          },
        );
      },
    );
  }

  @override
  void dispose() {
    super.dispose();
    for (var s in _appliancesStreams) {
      s.cancel();
    }
    EasyDebounce.cancel('firebase-appliances');
    _userAppliances.dispose();
    _userAppliancesAddedSub.cancel();
    _userAppliancesChangedSub.cancel();
    _userAppliancesRemovedSub.cancel();
  }

  void addAppliance(String name, String id) async => await _userAppliancesRef.child(name).set(id);
  void removeAppliance(String name) async => await _userAppliancesRef.child(name).remove();
  void updateAppliance(String name, String id) async => await _userAppliancesRef.child(name).set(id);
}
