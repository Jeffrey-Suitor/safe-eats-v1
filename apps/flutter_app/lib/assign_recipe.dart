import 'package:flutter/material.dart';
import 'package:safe_eats/themes/custom_colors.dart';
import 'package:provider/provider.dart';
import 'package:safe_eats/models/recipes_provider.dart';
import 'package:safe_eats/helpers.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:safe_eats/models/recipe_model.dart';
import 'package:firebase_database/firebase_database.dart';

class AssignRecipe extends StatefulWidget {
  const AssignRecipe({Key? key}) : super(key: key);

  @override
  State<AssignRecipe> createState() => _AssignRecipeState();
}

class _AssignRecipeState extends State<AssignRecipe> {
  String _recipe = '';

  @override
  Widget build(BuildContext context) {
    final String qrCode = ModalRoute.of(context)!.settings.arguments as String;
    return Scaffold(
      appBar: AppBar(
        title: const Text('Assign Recipe'),
      ),
      body: Consumer<RecipesProvider>(
        builder: (context, recipesProvider, child) {
          return Container(
            padding: const EdgeInsets.all(16),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text('Recipe QR Code: $qrCode'),
                const SizedBox(height: 20),
                const Text(
                  'Recipes',
                  style: TextStyle(fontSize: 24.0),
                ),
                const SizedBox(height: 20),
                ListView.separated(
                  shrinkWrap: true,
                  itemCount: recipesProvider.recipes.keys.toList().length,
                  itemBuilder: (context, index) {
                    return RadioListTile(
                      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                      title: Text(recipesProvider.recipes.values.toList()[index].name,
                          style: const TextStyle(fontSize: 20)),
                      isThreeLine: true,
                      subtitle: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const SizedBox(height: 4),
                          Text(recipesProvider.recipes.values.toList()[index].description),
                          const SizedBox(height: 4),
                          Row(
                            mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            children: [
                              Row(
                                children: [
                                  SvgPicture.asset(
                                    'assets/appliance.svg',
                                    color: CustomColors.black,
                                    height: 20,
                                  ),
                                  const SizedBox(width: 4),
                                  Text(recipesProvider.recipes.values.toList()[index].applianceType),
                                ],
                              ),
                              Row(
                                children: [
                                  SvgPicture.asset(
                                    'assets/clock.svg',
                                    color: CustomColors.black,
                                    height: 20,
                                  ),
                                  const SizedBox(width: 4),
                                  Text(intToTimeLeft(recipesProvider.recipes.values.toList()[index].duration)),
                                ],
                              ),
                            ],
                          ),
                        ],
                      ),
                      activeColor: CustomColors.primary,
                      value: recipesProvider.recipes.keys.toList()[index],
                      groupValue: _recipe,
                      onChanged: (String? value) {
                        setState(() {
                          _recipe = value as String;
                        });
                      },
                    );
                  },
                  separatorBuilder: (BuildContext context, num index) => const Divider(),
                ),
                const SizedBox(height: 20),
                const Spacer(),
                ElevatedButton(
                  style: ElevatedButton.styleFrom(primary: CustomColors.success),
                  child: const Text('Add Recipe'),
                  onPressed: () {
                    Navigator.pushNamed(context, '/add_recipe');
                  },
                ),
                const SizedBox(height: 20),
                ElevatedButton(
                  style: ElevatedButton.styleFrom(
                    minimumSize: const Size.fromHeight(70),
                  ),
                  child: const Text('Assign Recipe'),
                  onPressed: () {
                    if (_recipe.isEmpty) {
                      ScaffoldMessenger.of(context).showSnackBar(
                        const SnackBar(
                          content: Text('Please select a recipe'),
                        ),
                      );
                      return;
                    }
                    RecipeModel qrCodeRecipe = recipesProvider.recipes[_recipe] as RecipeModel;
                    final now = DateTime.now();
                    qrCodeRecipe.expiryDate =
                        (now.millisecondsSinceEpoch ~/ Duration.millisecondsPerSecond) + qrCodeRecipe.expiryDate;
                    DatabaseReference qrCodeRef = FirebaseDatabase.instance.ref('qrCodes/$qrCode');
                    qrCodeRef.set(qrCodeRecipe.toJson());
                    Navigator.popUntil(context, ModalRoute.withName('/scan_qr_code'));
                  },
                ),
                const SizedBox(height: 20),
                ElevatedButton(
                  style: ElevatedButton.styleFrom(primary: CustomColors.cancel),
                  child: const Text('Return'),
                  onPressed: () {
                    Navigator.popUntil(context, ModalRoute.withName('/scan_qr_code'));
                  },
                ),
              ],
            ),
          );
        },
      ),
    );
  }
}
