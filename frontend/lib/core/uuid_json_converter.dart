import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:uuid/uuid.dart';

class UuidJsonConverter implements JsonConverter<UuidValue, String> {
  const UuidJsonConverter();

  @override
  UuidValue fromJson(String json) {
    return UuidValue(json);
  }

  @override
  String toJson(UuidValue object) {
    return object.uuid;
  }
}
