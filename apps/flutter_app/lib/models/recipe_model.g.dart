// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'recipe_model.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

RecipeModel _$RecipeModelFromJson(Map<String, dynamic> json) => RecipeModel(
      applianceMode: json['applianceMode'] as String,
      applianceTemp: json['applianceTemp'] as num,
      applianceTempUnit: json['applianceTempUnit'] as String,
      applianceType: json['applianceType'] as String,
      description: json['description'] as String,
      duration: json['duration'] as num,
      expiryDate: json['expiryDate'] as num,
      id: json['id'] as String,
      name: json['name'] as String,
    );

Map<String, dynamic> _$RecipeModelToJson(RecipeModel instance) =>
    <String, dynamic>{
      'id': instance.id,
      'applianceMode': instance.applianceMode,
      'applianceTempUnit': instance.applianceTempUnit,
      'applianceType': instance.applianceType,
      'name': instance.name,
      'description': instance.description,
      'applianceTemp': instance.applianceTemp,
      'duration': instance.duration,
      'expiryDate': instance.expiryDate,
    };
