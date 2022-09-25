import 'package:json_annotation/json_annotation.dart';
import 'package:safe_eats/models/recipe_model.dart';
part 'appliance_model.g.dart';

@JsonSerializable()
class ApplianceModel {
  final String id, type;
  final bool isCooking;
  final num temperatureC, temperatureF, cookingStartTime, timestamp;
  RecipeModel? qrCode;

  ApplianceModel({
    required this.cookingStartTime,
    required this.timestamp,
    required this.id,
    required this.qrCode,
    required this.isCooking,
    required this.temperatureC,
    required this.temperatureF,
    required this.type,
  });

  factory ApplianceModel.fromJson(Map<String, dynamic> data) => _$ApplianceModelFromJson(data);
  Map<String, dynamic> toJson() => _$ApplianceModelToJson(this);
}
