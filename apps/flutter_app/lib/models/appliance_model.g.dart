// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'appliance_model.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

ApplianceModel _$ApplianceModelFromJson(Map<String, dynamic> json) =>
    ApplianceModel(
      cookingStartTime: json['cookingStartTime'] as num,
      timestamp: json['timestamp'] as num,
      id: json['id'] as String,
      qrCode: json['qrCode'] == null
          ? null
          : RecipeModel.fromJson(json['qrCode'] as Map<String, dynamic>),
      isCooking: json['isCooking'] as bool,
      temperatureC: json['temperatureC'] as num,
      temperatureF: json['temperatureF'] as num,
      type: json['type'] as String,
    );

Map<String, dynamic> _$ApplianceModelToJson(ApplianceModel instance) =>
    <String, dynamic>{
      'id': instance.id,
      'type': instance.type,
      'isCooking': instance.isCooking,
      'temperatureC': instance.temperatureC,
      'temperatureF': instance.temperatureF,
      'cookingStartTime': instance.cookingStartTime,
      'timestamp': instance.timestamp,
      'qrCode': instance.qrCode,
    };
