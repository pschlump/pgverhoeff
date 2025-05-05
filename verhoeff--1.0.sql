-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION verhoeff" to load this file. \quit

-- convet_to_title will compute a Verhoeff checksum and return a new string with this check digit appened.
CREATE FUNCTION validate_verhoeff(inp text) RETURNS boolean 
AS 'MODULE_PATHNAME', 'validate_verhoeff_check_digit'
LANGUAGE c STRICT VOLATILE;

-- convet_to_title will return true of the string has an accurate Verhoeff check digit at th end.
CREATE FUNCTION generate_verhoeff(inp text) RETURNS text 
AS 'MODULE_PATHNAME', 'append_verhoeff_check_digit'
LANGUAGE c STRICT VOLATILE;

