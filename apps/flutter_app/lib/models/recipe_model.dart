import 'package:json_annotation/json_annotation.dart';

part 'recipe_model.g.dart';

@JsonSerializable()
class RecipeModel {
  final String id, applianceMode, applianceTempUnit, applianceType, name, description;
  final num applianceTemp, duration;
  num expiryDate;

  RecipeModel({
    required this.applianceMode,
    required this.applianceTemp,
    required this.applianceTempUnit,
    required this.applianceType,
    required this.description,
    required this.duration,
    required this.expiryDate,
    required this.id,
    required this.name,
  });

  factory RecipeModel.fromJson(Map<String, dynamic> data) => _$RecipeModelFromJson(data);
  Map<String, dynamic> toJson() => _$RecipeModelToJson(this);
}
