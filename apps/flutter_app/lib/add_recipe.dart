import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:safe_eats/models/recipe_model.dart';
import 'package:safe_eats/models/recipes_provider.dart';
import 'package:safe_eats/themes/custom_colors.dart';

class AddRecipe extends StatefulWidget {
  final RecipeModel? newRecipe;

  const AddRecipe({
    Key? key,
    this.newRecipe,
  }) : super(key: key);

  @override
  State<AddRecipe> createState() => _AddRecipeState();
}

class _AddRecipeState extends State<AddRecipe> {
  final _formKey = GlobalKey<FormState>();

  Map<String, int> durationOptions = {
    'Secs': 1,
    'Mins': 60,
    'Hrs': 60 * 60,
    'Days': 60 * 60 * 24,
  };

  Map<String, int> expiryDateOptions = {
    'Mins': 60,
    'Hrs': 60 * 60,
    'Days': 60 * 60 * 24,
    'Weeks': 60 * 60 * 24 * 7,
    'Months': 60 * 60 * 24 * 7 * 30,
  };

  List<String> applianceTempOptions = ['F', 'C'];

  List<String> applianceTypeOptions = ['Toaster Oven'];
  Map<String, List<String>> applianceModeOptions = {
    'Toaster Oven': ['Bake', 'Broil', 'Rotisserie', 'Convection'],
  };

  late Map<String, Map<String, dynamic>> inputs = {
    'recipeName': {
      'hintText': 'Ex: Chicken Alfredo',
      'icon': 'assets/recipe.svg',
      'initialValue': widget.newRecipe?.name ?? '',
      'labelText': 'Recipe Name',
      'textEditingController': TextEditingController(),
      'validationMessage': 'Please enter a recipe name',
    },
    'description': {
      'hintText': 'Ex: This is a delicious chicken alfredo',
      'icon': 'assets/description.svg',
      'initialValue': widget.newRecipe?.description ?? '',
      'labelText': 'Description',
      'textEditingController': TextEditingController(),
      'validationMessage': 'Please enter a recipe description',
    },
    'duration': {
      'dropDownOptions': durationOptions.keys.toList(),
      'dropDownValue': durationOptions.keys.toList()[1],
      'hintText': 'Ex: 30',
      'icon': 'assets/clock.svg',
      'initialValue': widget.newRecipe?.duration ?? '',
      'labelText': 'Cooking Time',
      'textEditingController': TextEditingController(),
      'textInputType': TextInputType.number,
      'validationMessage': 'Please enter a recipe duration',
    },
    'expiryDate': {
      'dropDownOptions': expiryDateOptions.keys.toList(),
      'dropDownValue': expiryDateOptions.keys.toList()[2],
      'hintText': 'Ex: 14',
      'icon': 'assets/calendar.svg',
      'initialValue': widget.newRecipe?.expiryDate ?? '',
      'labelText': 'Expiry Date',
      'textEditingController': TextEditingController(),
      'textInputType': TextInputType.number,
      'validationMessage': 'Please enter a recipe expiry date',
    },
    'applianceType': {
      'icon': 'assets/appliance.svg',
      'dropDownOptions': applianceTypeOptions,
      'dropDownValue': widget.newRecipe?.applianceType ?? applianceTypeOptions[0],
    },
    'applianceTemp': {
      'dropDownOptions': applianceTempOptions,
      'dropDownValue': applianceTempOptions[0],
      'hintText': 'Ex: 350',
      'icon': 'assets/temperature.svg',
      'initialValue': widget.newRecipe?.applianceTemp ?? '',
      'labelText': 'Temperature',
      'textEditingController': TextEditingController(),
      'textInputType': TextInputType.number,
      'validationMessage': 'Please enter a temperature for recipe',
    },
    'applianceMode': {
      'icon': 'assets/appliance_mode.svg',
      'dropDownOptions': applianceModeOptions[widget.newRecipe?.applianceType ?? applianceTypeOptions[0]],
      'dropDownValue': applianceModeOptions[widget.newRecipe?.applianceType ?? applianceTypeOptions[0]]![0],
    },
  };

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Add Recipe'),
      ),
      body: Form(
        key: _formKey,
        child: SingleChildScrollView(
          padding: const EdgeInsets.all(16),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              ListView.separated(
                shrinkWrap: true,
                itemCount: inputs.length,
                itemBuilder: (context, index) {
                  String key = inputs.keys.elementAt(index);
                  return Row(
                    children: [
                      if (inputs[key]?['textEditingController'] != null)
                        Expanded(
                          flex: 2,
                          child: TextFormField(
                            keyboardType: inputs[key]?['textInputType'] ?? TextInputType.text,
                            textInputAction: index != inputs.length - 1 ? TextInputAction.next : TextInputAction.done,
                            validator: (value) {
                              if (value == null || value.isEmpty) {
                                return inputs[key]!['validationMessage'];
                              }
                              return null;
                            },
                            controller: inputs[key]!['textEditingController'],
                            decoration: InputDecoration(
                              hintText: inputs[key]!['hintText'],
                              labelText: inputs[key]!['labelText'],
                              prefixIcon: Align(
                                widthFactor: 1.0,
                                heightFactor: 1.0,
                                child: SvgPicture.asset(inputs[key]!['icon'],
                                    color: Theme.of(context).primaryColor,
                                    height: 24,
                                    width: 24,
                                    fit: BoxFit.scaleDown),
                              ),
                            ),
                          ),
                        ),
                      if (inputs[key]?['dropDownOptions'] != null && inputs[key]?['textEditingController'] != null)
                        const SizedBox(width: 16),
                      if (inputs[key]?['dropDownOptions'] != null)
                        Expanded(
                          child: DropdownButtonFormField<String>(
                            decoration: inputs[key]?['textEditingController'] == null
                                ? InputDecoration(
                                    prefixIcon: Align(
                                        widthFactor: 1.0,
                                        heightFactor: 1.0,
                                        child: SvgPicture.asset(inputs[key]!['icon'],
                                            color: Theme.of(context).primaryColor,
                                            height: 24,
                                            width: 24,
                                            fit: BoxFit.scaleDown)),
                                  )
                                : null,
                            value: inputs[key]!['dropDownValue'],
                            icon: const Icon(
                              Icons.keyboard_arrow_down,
                            ),
                            elevation: 16,
                            onChanged: (String? newValue) {
                              setState(() {
                                inputs[key]!['dropDownValue'] = newValue;
                              });
                            },
                            items: inputs[key]!['dropDownOptions'].map<DropdownMenuItem<String>>((String value) {
                              return DropdownMenuItem<String>(
                                value: value,
                                child: Text(value),
                              );
                            }).toList(),
                          ),
                        ),
                    ],
                  );
                },
                separatorBuilder: (BuildContext context, num index) => const Divider(),
              ),
              const SizedBox(height: 20),
              ElevatedButton(
                style: ElevatedButton.styleFrom(
                  minimumSize: const Size.fromHeight(70),
                ),
                child: const Text('Add Recipe'),
                onPressed: () async {
                  if (_formKey.currentState!.validate()) {
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(
                        content: Text('Uploading Recipe'),
                      ),
                    );
                    final String uid = FirebaseAuth.instance.currentUser!.uid;
                    final String time = DateTime.now().millisecondsSinceEpoch.toString();
                    final String recipeName = inputs['recipeName']!['textEditingController'].text;
                    final String id = widget.newRecipe?.id ?? '$uid-$recipeName-$time';

                    final num durMult = durationOptions[inputs['duration']!['dropDownValue']] as int;
                    final num duration = int.parse(inputs['duration']!['textEditingController'].text) * durMult;

                    final num expMult = expiryDateOptions[inputs['expiryDate']!['dropDownValue']] as int;
                    final num expiryDate = int.parse(inputs['expiryDate']!['textEditingController'].text) * expMult;

                    final RecipeModel finalizedRecipe = RecipeModel(
                      applianceMode: inputs['applianceMode']!['dropDownValue'],
                      applianceTemp: int.parse(inputs['applianceTemp']!['textEditingController'].text),
                      applianceTempUnit: inputs['applianceTemp']!['dropDownValue'],
                      applianceType: inputs['applianceType']!['dropDownValue'],
                      description: inputs['description']!['textEditingController'].text,
                      duration: duration,
                      expiryDate: expiryDate,
                      id: id,
                      name: recipeName,
                    );
                    Provider.of<RecipesProvider>(context, listen: false).addRecipe(finalizedRecipe);
                    Navigator.pop(context);
                  }
                },
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
      ),
    );
  }
}
