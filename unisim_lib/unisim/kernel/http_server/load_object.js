function load_object(main_href,object_name)
{
	tartanpion.location.href = main_href;
	config.location.href = '/config?object=' + object_name;
	stats.location.href = '/stats?object=' + object_name;
	location.href = '/?object=' + object_name;
}
