// MDN Logger

//
// To correctly use the logger you MUST correctly set, 
// according to the actual configuration, the values of: 
//
//  logName   -> the name of the log set in the logconfig file
//	logUrl    -> the correct url of the js_logger.php script 
//

var logName ;//= 'Default' ;

var logUrl = '/risorse/xml-php/js_logger.php';

var log_id = 0;

// Call this function passing the msg (it works also with array)
// to log it.

function log_value($msg){
  log_id++;

  var url = logUrl +  '?msg=' + escape($msg)
          + '&logid=' + log_id + '&pageaddr=' + escape(location.href);
  if (logName != undefined){
       url = url + '&logname=' + logName;
       //alert(url);
  }
	window.open(url,'','status=no,width=150,height=100');

}
