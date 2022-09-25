import 'dart:async';
import 'dart:convert';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:safe_eats/models/recipe_model.dart';

class RecipesProvider with ChangeNotifier {
  final Map<String, RecipeModel> _recipes = {};
  final String _uid = FirebaseAuth.instance.currentUser!.uid;
  late final StreamSubscription<DatabaseEvent> _recipesAddedSub;
  late final StreamSubscription<DatabaseEvent> _recipesChangedSub;
  late final StreamSubscription<DatabaseEvent> _recipesRemovedSub;
  late final DatabaseReference recipesRef = FirebaseDatabase.instance.ref('users/$_uid/recipes');

  Map<String, RecipeModel> get recipes => _recipes;

  RecipesProvider() {
    _listenRecipesStream();
  }

  void _listenRecipesStream() {
    _recipesAddedSub = recipesRef.onChildAdded.listen(
      (DatabaseEvent event) {
        if (event.snapshot.value != null) {
          _recipes[event.snapshot.key as String] = RecipeModel.fromJson(jsonDecode(jsonEncode(event.snapshot.value)));
          notifyListeners();
        }
      },
      onError: (Object o) {
        final error = o as FirebaseException;
        debugPrint('Error: ${error.code} ${error.message}');
      },
    );

    _recipesChangedSub = recipesRef.onChildChanged.listen(
      (DatabaseEvent event) {
        if (event.snapshot.value != null) {
          _recipes[event.snapshot.key as String] = RecipeModel.fromJson(jsonDecode(jsonEncode(event.snapshot.value)));
          notifyListeners();
        }
      },
      onError: (Object o) {
        final error = o as FirebaseException;
        debugPrint('Error: ${error.code} ${error.message}');
      },
    );

    _recipesRemovedSub = recipesRef.onChildRemoved.listen(
      (DatabaseEvent event) {
        _recipes.remove(event.snapshot.key as String);
        notifyListeners();
      },
      onError: (Object o) {
        final error = o as FirebaseException;
        debugPrint('Error: ${error.code} ${error.message}');
      },
    );
  }

  @override
  void dispose() {
    super.dispose();
    _recipesAddedSub.cancel();
    _recipesChangedSub.cancel();
    _recipesRemovedSub.cancel();
  }

  void addRecipe(RecipeModel recipe) async => await recipesRef.child(recipe.id).set(recipe.toJson());
  void removeRecipe(RecipeModel recipe) async => await recipesRef.child(recipe.id).remove();
  void updateRecipe(RecipeModel recipe) async => await recipesRef.child(recipe.id).update(recipe.toJson());
}
