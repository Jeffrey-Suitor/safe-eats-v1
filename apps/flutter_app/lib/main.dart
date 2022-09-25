import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:safe_eats/add_appliance.dart';
import 'package:safe_eats/add_recipe.dart';
import 'package:safe_eats/auth.dart';
import 'package:safe_eats/firebase_options.dart';
import 'package:safe_eats/models/appliances_provider.dart';
import 'package:safe_eats/models/recipes_provider.dart';
import 'package:safe_eats/scan_qr_code.dart';
import 'package:safe_eats/themes/custom_theme.dart';
import 'package:safe_eats/assign_recipe.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
  // FirebaseDatabase.instance.setPersistenceEnabled(true);
  runApp(
    MultiProvider(providers: [
      ChangeNotifierProvider(create: (_) => RecipesProvider()),
      ChangeNotifierProvider(create: (_) => AppliancesProvider()),
    ], child: const MyApp()),
  );
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: CustomTheme.lightTheme,

      // Start the app with the "/" named route. In this case, the app starts
      // on the FirstScreen widget.
      initialRoute: '/auth',
      routes: {
        '/auth': (context) => const AuthGate(),
        '/add_appliance': (context) => const AddAppliance(),
        '/add_recipe': (context) => const AddRecipe(),
        '/scan_qr_code': (context) => const ScanQrCode(),
        '/assign_recipe': (context) => const AssignRecipe(),
      },
    );
  }
}
