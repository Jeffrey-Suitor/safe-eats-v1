import 'package:json_annotation/json_annotation.dart';

part 'user_model.g.dart';

@JsonSerializable()
class UserModel {
  final String firstName, lastName, add;
  final bool subscription;

  UserModel({
    required this.firstName,
    required this.lastName,
    required this.add,
    required this.subscription,
  });

  factory UserModel.fromJson(Map<String, dynamic> data) => _$UserModelFromJson(data);
  Map<String, dynamic> toJson() => _$UserModelToJson(this);
}
