import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutterfire_ui/auth.dart';
import 'package:safe_eats/homescreen.dart';

class AuthGate extends StatelessWidget {
  const AuthGate({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<User?>(
      stream: FirebaseAuth.instance.authStateChanges(),
      builder: (context, snapshot) {
        if (snapshot.connectionState != ConnectionState.active) {
          return const Center(child: CircularProgressIndicator());
        }
        // User is not signed in
        if (!snapshot.hasData) {
          return Center(
            child: SignInScreen(
            headerBuilder: (context, constraints, _) {
              return Padding(
                  padding: const EdgeInsets.all(20),
                  child: AspectRatio(
                    aspectRatio: 1,
                    child: Image.asset('assets/logo.png'),
                  ));
            },
            providerConfigs: const [
              EmailProviderConfiguration(),
              // GoogleProviderConfiguration(
              //   clientId: '352944254709-2pup234mne66sl92ok9lm720ragdg6av.apps.googleusercontent.com',
              // ),
            ],
          ),);
        }

        // Render your application if authenticated
        return const HomeScreen();
      },
    );
  }
}
