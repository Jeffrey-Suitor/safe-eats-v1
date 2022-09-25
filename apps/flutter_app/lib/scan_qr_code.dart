import 'package:flutter/material.dart';
import 'package:safe_eats/themes/custom_colors.dart';
import 'package:mobile_scanner/mobile_scanner.dart';

class ScanQrCode extends StatefulWidget {
  const ScanQrCode({Key? key}) : super(key: key);

  @override
  State<ScanQrCode> createState() => _ScanQrCodeState();
}

class _ScanQrCodeState extends State<ScanQrCode> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Scan Recipe QR Code'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          children: [
            Expanded(
              child: MobileScanner(
                fit: BoxFit.contain,
                allowDuplicates: false,
                controller: MobileScannerController(),
                onDetect: (qrCode, args) {
                  final String qrValues = qrCode.rawValue as String;
                  final bool isSafeEats = qrValues.split(":")[0] == "SafeEatsRecipeQRCode";
                  if (isSafeEats) {
                    Navigator.of(context).pushNamed('/assign_recipe', arguments: qrValues.split(":")[1]);
                  } else {
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(
                        content: Text('Invalid Recipe QR Code'),
                      ),
                    );
                  }
                },
              ),
            ),
            ElevatedButton(
              style: ElevatedButton.styleFrom(primary: CustomColors.cancel),
              child: const Text('Return'),
              onPressed: () {
                Navigator.pop(context);
              },
            ),
          ],
        ),
      ),
    );
  }
}
