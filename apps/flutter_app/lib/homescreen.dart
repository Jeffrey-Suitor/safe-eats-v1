import 'package:flutter/material.dart';
import 'package:flutter_speed_dial/flutter_speed_dial.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:safe_eats/models/appliances_provider.dart';
import 'package:safe_eats/models/recipes_provider.dart';
import 'package:provider/provider.dart';
import 'package:safe_eats/themes/custom_colors.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';
import 'package:safe_eats/models/appliance_model.dart';
import 'package:safe_eats/models/recipe_model.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:safe_eats/helpers.dart';
import 'package:intl/intl.dart';

enum Pages {
  appliances,
  recipes,
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final ValueNotifier<bool> _isDialOpen = ValueNotifier<bool>(false);
  Pages _selectedPage = Pages.appliances;

  @override
  void dispose() {
    _isDialOpen.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Home'), actions: [
        // Create a logout button using flutter fire auth
        IconButton(
            icon: const Icon(Icons.exit_to_app),
            onPressed: () async {
              await FirebaseAuth.instance.signOut();
              Navigator.of(context).pushReplacementNamed('/auth');
            })
      ]),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Center(
          child: Builder(builder: (context) {
            if (_selectedPage == Pages.appliances) {
              return Consumer<AppliancesProvider>(
                builder: (context, appliancesProvider, child) {
                  List<String> applianceNames = appliancesProvider.appliances.keys.toList();
                  List<ApplianceModel> appliances = appliancesProvider.appliances.values.toList();
                  if (appliancesProvider.appliances.keys.toList().isEmpty) {
                    return Center(
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          const Text(
                            'No appliances found',
                            style: TextStyle(fontSize: 20, color: CustomColors.text),
                          ),
                          const SizedBox(height: 20),
                          ElevatedButton(
                            child: const Text('Add Appliance'),
                            onPressed: () {
                              Navigator.pushNamed(context, '/add_appliance');
                            },
                          ),
                        ],
                      ),
                    );
                  }
                  return ListView.separated(
                    shrinkWrap: true,
                    itemCount: applianceNames.length,
                    itemBuilder: (context, index) {
                      final ApplianceModel appliance = appliances[index];
                      final bool isCelcius = appliance.qrCode?.applianceTempUnit == "C";
                      final num temp = isCelcius ? appliance.temperatureC : appliances[index].temperatureF;
                      final String tempUnit = isCelcius ? "°C" : "°F";
                      final bool isCooking = appliance.isCooking;
                      final num endTime = appliance.cookingStartTime + (appliance.qrCode?.duration ?? 0);
                      final num currentTime = DateTime.now().millisecondsSinceEpoch ~/ 1000;
                      debugPrint('currentTime: $currentTime endTime: $endTime diff: ${endTime - currentTime}');
                      debugPrint('diff: ${100 - (endTime - currentTime) / (appliance.qrCode?.duration ?? 1) * 100}');
                      return ExpansionTile(
                        iconColor: CustomColors.primary,
                        collapsedIconColor: CustomColors.primary,
                        textColor: CustomColors.text,
                        title: Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            SvgPicture.asset('assets/appliance.svg',
                                color: CustomColors.text, height: 20, width: 16, fit: BoxFit.scaleDown),
                            const SizedBox(width: 8),
                            Text(
                              applianceNames[index],
                              textAlign: TextAlign.center,
                              style: const TextStyle(fontSize: 20, color: CustomColors.text),
                            ),
                          ],
                        ),
                        subtitle: SizedBox(
                          height: 200,
                          child: Row(
                            // mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            children: [
                              Expanded(
                                child: SfRadialGauge(
                                  axes: <RadialAxis>[
                                    RadialAxis(
                                      canScaleToFit: isCooking ? false : true,
                                      showTicks: false,
                                      startAngle: 180,
                                      endAngle: 0,
                                      minimum: 0,
                                      maximum: appliance.qrCode?.applianceTemp.toDouble() ?? 380.0 + 20.0,
                                      radiusFactor: 0.85,
                                      axisLineStyle:
                                          const AxisLineStyle(thicknessUnit: GaugeSizeUnit.factor, thickness: 0.15),
                                      annotations: <GaugeAnnotation>[
                                        GaugeAnnotation(
                                          angle: 180,
                                          widget: Row(
                                            mainAxisSize: MainAxisSize.min,
                                            children: [
                                              SvgPicture.asset('assets/temperature.svg',
                                                  color: CustomColors.text,
                                                  height: 20,
                                                  width: 16,
                                                  fit: BoxFit.scaleDown),
                                              const SizedBox(width: 8),
                                              Text('$temp$tempUnit'),
                                            ],
                                          ),
                                        ),
                                      ],
                                      pointers: <GaugePointer>[
                                        RangePointer(
                                            value: temp.toDouble(),
                                            cornerStyle: CornerStyle.bothCurve,
                                            enableAnimation: true,
                                            animationDuration: 1200,
                                            animationType: AnimationType.ease,
                                            sizeUnit: GaugeSizeUnit.factor,
                                            color: appliance.isCooking ? CustomColors.primary : CustomColors.disable,
                                            width: 0.15),
                                      ],
                                    ),
                                  ],
                                ),
                              ),
                              if (isCooking)
                                Expanded(
                                  child: SfRadialGauge(
                                    axes: <RadialAxis>[
                                      RadialAxis(
                                        canScaleToFit: true,
                                        showTicks: false,
                                        showLabels: false,
                                        startAngle: 270,
                                        endAngle: 270,
                                        minimum: 0,
                                        maximum: 100,
                                        radiusFactor: 0.85,
                                        axisLineStyle:
                                            const AxisLineStyle(thicknessUnit: GaugeSizeUnit.factor, thickness: 0.15),
                                        annotations: <GaugeAnnotation>[
                                          GaugeAnnotation(
                                            angle: 180,
                                            widget: Row(
                                              mainAxisSize: MainAxisSize.min,
                                              children: [
                                                SvgPicture.asset('assets/clock.svg',
                                                    color: CustomColors.text,
                                                    height: 20,
                                                    width: 16,
                                                    fit: BoxFit.scaleDown),
                                                const SizedBox(width: 8),
                                                Text(intToTimeLeft(endTime - currentTime)),
                                              ],
                                            ),
                                          ),
                                        ],
                                        pointers: <GaugePointer>[
                                          RangePointer(
                                              value: (endTime - currentTime) / (appliance.qrCode?.duration ?? 1) * 100,
                                              cornerStyle: CornerStyle.bothCurve,
                                              enableAnimation: true,
                                              animationDuration: 1200,
                                              animationType: AnimationType.ease,
                                              sizeUnit: GaugeSizeUnit.factor,
                                              color: appliance.isCooking ? CustomColors.primary : CustomColors.disable,
                                              width: 0.15),
                                        ],
                                      ),
                                    ],
                                  ),
                                ),
                            ],
                          ),
                        ),
                        children: <Widget>[
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/temperature.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Text('Temperature: ${appliance.temperatureF} °F'),
                              ],
                            ),
                          ),
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/temperature.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Text('Temperature: ${appliance.temperatureC} °C'),
                              ],
                            ),
                          ),
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/appliance.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Text('Appliance: ${appliance.type}'),
                              ],
                            ),
                          ),
                          if (isCooking) ...[
                            ListTile(
                              title: Row(
                                children: [
                                  SvgPicture.asset('assets/recipe.svg',
                                      color: CustomColors.text, height: 16, width: 16),
                                  const SizedBox(width: 8),
                                  Text('Recipe Name: ${appliance.qrCode?.name}'),
                                ],
                              ),
                            ),
                            ListTile(
                              title: Row(
                                children: [
                                  SvgPicture.asset('assets/temperature.svg',
                                      color: CustomColors.text, height: 16, width: 16),
                                  const SizedBox(width: 8),
                                  Text(
                                    'Recipe Temp: ${appliance.qrCode?.applianceTemp}°${appliance.qrCode?.applianceTempUnit}',
                                  ),
                                ],
                              ),
                            ),
                            ListTile(
                              title: Row(
                                children: [
                                  SvgPicture.asset('assets/calendar.svg',
                                      color: CustomColors.text, height: 16, width: 16),
                                  const SizedBox(width: 8),
                                  Builder(builder: (context) {
                                    var date = DateTime.fromMillisecondsSinceEpoch(
                                        appliance.qrCode!.expiryDate.toInt() * 1000);
                                    String formattedDate = DateFormat('yyyy-MM-dd – kk:mm:ss').format(date);
                                    return Text('Expiry Date: $formattedDate');
                                  })
                                ],
                              ),
                            ),
                          ],
                          ListTile(
                            title: Row(
                              children: [
                                const Icon(Icons.sync, color: CustomColors.text, size: 16),
                                const SizedBox(width: 8),
                                Builder(builder: (context) {
                                  var date = DateTime.fromMillisecondsSinceEpoch(appliance.timestamp.toInt() * 1000);
                                  String formattedDate = DateFormat('yyyy-MM-dd – kk:mm:ss').format(date);
                                  return Text('Last Update: $formattedDate');
                                })
                              ],
                            ),
                          ),
                        ],
                      );
                    },
                    separatorBuilder: (BuildContext context, num index) => const Divider(),
                  );
                },
              );
            }
            if (_selectedPage == Pages.recipes) {
              return Consumer<RecipesProvider>(
                builder: (context, recipesProvider, child) {
                  if (recipesProvider.recipes.keys.toList().isEmpty) {
                    return Column(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [
                        const Text(
                          'No recipes found',
                          style: TextStyle(fontSize: 20, color: CustomColors.text),
                        ),
                        const SizedBox(height: 20),
                        ElevatedButton(
                          child: const Text('Add Recipe'),
                          onPressed: () {
                            Navigator.pushNamed(context, '/add_recipe');
                          },
                        ),
                      ],
                    );
                  }
                  return ListView.separated(
                    shrinkWrap: true,
                    itemCount: recipesProvider.recipes.keys.toList().length,
                    itemBuilder: (context, index) {
                      final List<RecipeModel> recipeList = recipesProvider.recipes.values.toList();
                      return ExpansionTile(
                        iconColor: CustomColors.primary,
                        collapsedIconColor: CustomColors.primary,
                        title: Row(
                          children: [
                            SvgPicture.asset('assets/recipe.svg', color: CustomColors.text, height: 20, width: 20),
                            const SizedBox(width: 8),
                            Text(
                              recipeList[index].name,
                              style: const TextStyle(fontSize: 20, color: CustomColors.text),
                            ),
                          ],
                        ),
                        subtitle: Row(
                          children: [
                            SvgPicture.asset('assets/description.svg', color: CustomColors.text, height: 16, width: 16),
                            const SizedBox(width: 8),
                            Text(
                              recipeList[index].description,
                              style: const TextStyle(color: CustomColors.text),
                            ),
                          ],
                        ),
                        children: <Widget>[
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/clock.svg', color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Builder(
                                  builder: (context) {
                                    final Duration duration = Duration(seconds: recipeList[index].duration.toInt());
                                    return Text(
                                      'Cooking Time (Hrs:Min): ${duration.inHours}:${duration.inMinutes}',
                                    );
                                  },
                                ),
                              ],
                            ),
                          ),
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/calendar.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Builder(
                                  builder: (context) {
                                    debugPrint(recipeList[index].expiryDate.toString());
                                    final Duration expiryDate = Duration(seconds: recipeList[index].expiryDate.toInt());
                                    debugPrint(expiryDate.inSeconds.toString());
                                    return Text('Expiry Date (Days): ${expiryDate.inDays}');
                                  },
                                ),
                              ],
                            ),
                          ),
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/appliance.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Text('Appliance: ${recipeList[index].applianceType}'),
                              ],
                            ),
                          ),
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/temperature.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Text(
                                    'Cooking Temperature: ${recipeList[index].applianceTemp} °${recipeList[index].applianceTempUnit}'),
                              ],
                            ),
                          ),
                          ListTile(
                            title: Row(
                              children: [
                                SvgPicture.asset('assets/appliance_mode.svg',
                                    color: CustomColors.text, height: 16, width: 16),
                                const SizedBox(width: 8),
                                Text('Mode: ${recipeList[index].applianceMode}'),
                              ],
                            ),
                          ),
                          ElevatedButton(
                            child: const Text('Edit Recipe'),
                            onPressed: () {
                              Navigator.pushNamed(context, '/add_recipe', arguments: recipeList[index]);
                            },
                          ),
                        ],
                      );
                    },
                    separatorBuilder: (BuildContext context, num index) => const Divider(),
                  );
                },
              );
            }
            return const Text("Error page not found");
          }),
        ),
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.centerDocked,
      floatingActionButton: SpeedDial(
        elevation: 0,
        icon: Icons.add,
        activeIcon: Icons.close,
        openCloseDial: _isDialOpen,
        spacing: 3,
        spaceBetweenChildren: 4,
        childPadding: const EdgeInsets.all(5),
        buttonSize: const Size(80, 80),
        activeLabel: const Text('Close'),
        childrenButtonSize: const Size(80, 80),
        backgroundColor: Theme.of(context).primaryColor,
        children: [
          SpeedDialChild(
            child: SvgPicture.asset('assets/qr_code.svg'),
            backgroundColor: Theme.of(context).primaryColor,
            label: 'Scan QR Code',
            onTap: () => Navigator.pushNamed(context, '/scan_qr_code'),
          ),
          SpeedDialChild(
            child: SvgPicture.asset('assets/recipe.svg'),
            backgroundColor: Theme.of(context).primaryColor,
            label: 'Add Recipe',
            onTap: () => Navigator.pushNamed(context, '/add_recipe'),
          ),
          SpeedDialChild(
            child: SvgPicture.asset('assets/appliance.svg'),
            backgroundColor: Theme.of(context).primaryColor,
            label: 'Add Appliance',
            onTap: () => Navigator.pushNamed(context, '/add_appliance'),
          ),
        ],
      ),
      bottomNavigationBar: BottomAppBar(
        shape: const CircularNotchedRectangle(),
        notchMargin: 8.0,
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceAround,
          children: <Widget>[
            Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                IconButton(
                  tooltip: 'View appliances',
                  icon: SvgPicture.asset('assets/appliance.svg',
                      color: _selectedPage == Pages.appliances ? Theme.of(context).primaryColor : CustomColors.disable),
                  onPressed: () {
                    setState(() {
                      _selectedPage = Pages.appliances;
                    });
                  },
                ),
                Text(
                  "Appliances",
                  style: TextStyle(
                      color: _selectedPage == Pages.appliances ? Theme.of(context).primaryColor : CustomColors.disable),
                ),
              ],
            ),
            const SizedBox(width: 80, height: 80),
            Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                IconButton(
                  tooltip: 'View recipes',
                  icon: SvgPicture.asset('assets/recipe.svg',
                      color: _selectedPage == Pages.recipes ? Theme.of(context).primaryColor : CustomColors.disable),
                  onPressed: () {
                    setState(() {
                      _selectedPage = Pages.recipes;
                    });
                  },
                ),
                Text(
                  "Recipes",
                  style: TextStyle(
                      color: _selectedPage == Pages.recipes ? Theme.of(context).primaryColor : CustomColors.disable),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
