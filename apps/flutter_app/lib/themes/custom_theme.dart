import 'package:flutter/material.dart';
import 'package:safe_eats/themes/custom_colors.dart';

class CustomTheme {
  static ThemeData get lightTheme {
    return ThemeData(
      primaryColor: CustomColors.primary,
      scaffoldBackgroundColor: Colors.white,
      fontFamily: 'Montserrat',
      appBarTheme: const AppBarTheme(
        backgroundColor: CustomColors.primary,
      ),
      inputDecorationTheme: const InputDecorationTheme(
        labelStyle: TextStyle(color: CustomColors.text, fontSize: 16.0),
        focusedBorder: OutlineInputBorder(
          borderSide: BorderSide(color: CustomColors.primary),
        ),
        enabledBorder: OutlineInputBorder(
          borderSide: BorderSide(color: CustomColors.disable),
        ),
      ),
      elevatedButtonTheme: ElevatedButtonThemeData(
        style: ElevatedButton.styleFrom(
          primary: CustomColors.primary,
          minimumSize: const Size.fromHeight(50),
        ),
      ),
      outlinedButtonTheme: OutlinedButtonThemeData(
        style: ButtonStyle(
          padding: MaterialStateProperty.all<EdgeInsets>(
            const EdgeInsets.all(24),
          ),
          backgroundColor: MaterialStateProperty.all<Color>(CustomColors.primary),
          foregroundColor: MaterialStateProperty.all<Color>(Colors.white),
        ),
      ),
      textButtonTheme: TextButtonThemeData(
        style: TextButton.styleFrom(
          primary: CustomColors.primary,
        ),
      ),
    );
  }
}
