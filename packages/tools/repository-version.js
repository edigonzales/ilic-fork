import {
  selectLatestModelVersion as selectCoreModelVersion,
  supportedSchemaLanguagePreference
} from "@ilic/repository-core";

export function selectLatestModelVersion(models, name, schemaLanguage = "", onWarning) {
  return selectCoreModelVersion(models, name, schemaLanguage,
    diagnostic => onWarning?.(diagnostic.message));
}

export { supportedSchemaLanguagePreference };
