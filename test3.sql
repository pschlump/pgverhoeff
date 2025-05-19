
-- A full block of tests that results in 'pass'/'fail' 

DO $$
DECLARE
 	l_txt text;
 	l_ok boolean;
 	n_err int;
	l_msg text;
BEGIN

 	n_err = 0;

	SELECT generate_verhoeff('12345')
		into l_txt;
	if l_txt != '123451' then
		n_err = n_err + 1;
	end if;

	SELECT validate_verhoeff('123451')
		into l_ok;
	if not l_ok then
		n_err = n_err + 1;
	end if;

	SELECT validate_verhoeff('123452')
		into l_ok;
	if l_ok then
		n_err = n_err + 1;
	end if;

	SELECT generate_verhoeff('12a45')
		into l_txt;
	if l_txt != '12a45!' then
		n_err = n_err + 1;
	end if;

	SELECT validate_verhoeff('12a452')
		into l_ok;
	if l_ok then
		n_err = n_err + 1;
	end if;

	if n_err = 0 then
		RAISE NOTICE 'PASS';
	else 
		RAISE NOTICE 'FAIL';
	end if;

END;
$$ LANGUAGE plpgsql;

