import 'package:flutter_svg/flutter_svg.dart';
import 'package:flutter/material.dart';
import 'package:mobile_scanner/mobile_scanner.dart';
import 'package:provider/provider.dart';
import 'package:safe_eats/models/appliances_provider.dart';
import 'package:safe_eats/themes/custom_colors.dart';

class AddAppliance extends StatefulWidget {
  const AddAppliance({Key? key}) : super(key: key);

  @override
  State<AddAppliance> createState() => _AddApplianceState();
}

class _AddApplianceState extends State<AddAppliance> {
  bool _isNotScanned = true;
  String _qrCode = '';
  final TextEditingController _controller = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Scan QR Code On Appliance'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          children: [
            !_isNotScanned
                ? Expanded(
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Appliance QR Code: $_qrCode'),
                        TextField(
                          controller: _controller,
                          decoration: InputDecoration(
                            hintText: 'Mom\'s toaster oven',
                            labelText: 'Appliance name',
                            prefixIcon: Align(
                              widthFactor: 1.0,
                              heightFactor: 1.0,
                              child: SvgPicture.asset('assets/appliance.svg',
                                  color: Theme.of(context).primaryColor, height: 24, width: 24, fit: BoxFit.scaleDown),
                            ),
                          ),
                        ),
                        ElevatedButton(
                          style: ElevatedButton.styleFrom(
                            minimumSize: const Size.fromHeight(70),
                          ),
                          child: const Text('Assign Appliance Name'),
                          onPressed: () {
                            if (_controller.text == '') {
                              ScaffoldMessenger.of(context).showSnackBar(
                                const SnackBar(
                                  content: Text('Please input an appliance name'),
                                ),
                              );
                              return;
                            }
                            Provider.of<AppliancesProvider>(context, listen: false)
                                .addAppliance(_controller.text, _qrCode);
                            Navigator.pop(context);
                          },
                        ),
                      ],
                    ),
                  )
                : Expanded(
                    child: MobileScanner(
                      fit: BoxFit.contain,
                      allowDuplicates: false,
                      controller: MobileScannerController(),
                      onDetect: (scannedCode, args) {
                        _qrCode = scannedCode.rawValue as String;
                        _isNotScanned = _qrCode.split(":")[0] == "SafeEatsApplianceQRCode";
                        if (!_isNotScanned) {
                          ScaffoldMessenger.of(context).showSnackBar(
                            const SnackBar(
                              content: Text('Invalid Appliance QR Code'),
                            ),
                          );
                        } else {
                          setState(() {
                            _qrCode = _qrCode.split(":")[1];
                            _isNotScanned = false;
                          });
                        }
                      },
                    ),
                  ),
            const SizedBox(height: 20),
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
