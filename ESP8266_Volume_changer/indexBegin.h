const char indexBegin[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, height=device-height, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
    <title>SmartWeather</title>
    <style>@font-face{font-family:Montserrat;font-style:normal;font-weight:400;src:local('Montserrat Regular'),local('Montserrat-Regular'),url(https://fonts.gstatic.com/s/montserrat/v14/JTUSjIg1_i6t8kCHKm459W1hyzbi.woff2) format('woff2');unicode-range:U+0400-045F,U+0490-0491,U+04B0-04B1,U+2116}@font-face{font-family:Montserrat;font-style:normal;font-weight:400;src:local('Montserrat Regular'),local('Montserrat-Regular'),url(https://fonts.gstatic.com/s/montserrat/v14/JTUSjIg1_i6t8kCHKm459WZhyzbi.woff2) format('woff2');unicode-range:U+0102-0103,U+0110-0111,U+0128-0129,U+0168-0169,U+01A0-01A1,U+01AF-01B0,U+1EA0-1EF9,U+20AB}@font-face{font-family:Montserrat;font-style:normal;font-weight:400;src:local('Montserrat Regular'),local('Montserrat-Regular'),url(https://fonts.gstatic.com/s/montserrat/v14/JTUSjIg1_i6t8kCHKm459Wdhyzbi.woff2) format('woff2');unicode-range:U+0100-024F,U+0259,U+1E00-1EFF,U+2020,U+20A0-20AB,U+20AD-20CF,U+2113,U+2C60-2C7F,U+A720-A7FF}@font-face{font-family:Montserrat;font-style:normal;font-weight:400;src:local('Montserrat Regular'),local('Montserrat-Regular'),url(https://fonts.gstatic.com/s/montserrat/v14/JTUSjIg1_i6t8kCHKm459Wlhyw.woff2) format('woff2');unicode-range:U+0000-00FF,U+0131,U+0152-0153,U+02BB-02BC,U+02C6,U+02DA,U+02DC,U+2000-206F,U+2074,U+20AC,U+2122,U+2191,U+2193,U+2212,U+2215,U+FEFF,U+FFFD}*{-moz-user-select:none;-khtml-user-select:none;-webkit-user-select:none;-ms-user-select:none;user-select:none}body{margin:0;min-height:100vh;font:16px/1 sans-serif;display:flex;flex-direction:column;align-content:center;justify-content:center;font-family:Montserrat,sans-serif;background-repeat:no-repeat;background-image:linear-gradient(-30deg,#a8edff 0,#fbfcdb 100%)}h1{margin-top:0;margin-bottom:0;margin-left:auto;margin-right:auto;padding-top:40px;font-size:50px}h3{text-align:center}h4{margin:auto;padding-top:1em}form{margin:0;margin-left:auto;margin-right:auto;max-width:90%;width:420px}input{width:400px;margin:2%;max-width:96%}.alpha{background-image:linear-gradient(to right,hsla(66,31%,94%,.18),#fff);border-radius:3px;border-style:solid;border-width:thin;border-color:Grey}input[type=text],select{padding:12px 20px;display:inline-block;border:1px solid #ccc;box-sizing:border-box;font-weight:700}input[type=button],select{width:100%;background-color:#add8e6;color:#fff;padding:14px 20px;border:none;border-radius:4px;cursor:pointer;font-weight:700;font-size:18px}input[type=password],select{padding:12px 20px;display:inline-block;border:1px solid #ccc;box-sizing:border-box;font-weight:700}.button{width:100%;background-color:grey;color:#fff;padding:14px 1px;border:none;border-radius:4px;cursor:pointer;font-weight:700;transition:all .8s}.button:hover,input[type=button]:hover{-webkit-box-shadow:0 5px 30px -10px rgba(0,0,0,.57);-moz-box-shadow:0 5px 30px -10px rgba(0,0,0,.57);box-shadow:5px 30px -10px rgba(0,0,0,.57);transition:all .2s}.button:active{background-image:#00f}.buttonMarg{margin-left:10px;margin-right:10px}.left{display:table-cell;width:80%;margin-left:5%}.center{display:table-cell;text-align:center;width:5%}.right{display:table-cell;text-align:right;width:15%}#avNetworks{padding:0}.animate-bottom{position:relative;-webkit-animation-name:animatebottom;-webkit-animation-duration:1s;animation-name:animatebottom;animation-duration:1s}@-webkit-keyframes animatebottom{from{bottom:-100px;opacity:0}to{bottom:0;opacity:1}}@keyframes animatebottom{from{bottom:-100px;opacity:0}to{bottom:0;opacity:1}}.animate-top{position:relative;-webkit-animation-name:animatetop;-webkit-animation-duration:1s;animation-name:animatetop;animation-duration:1s}@-webkit-keyframes animatetop{from{bottom:100px;opacity:0}to{bottom:0;opacity:1}}@keyframes animatetop{from{bottom:100px;opacity:0}to{bottom:0;opacity:1}}#smartWeatherConnect{padding:0}.lds-ring{align-items:center;display:inline-block;position:relative;width:64px;height:64px}.lds-ring div{box-sizing:border-box;display:block;position:absolute;width:51px;height:51px;margin:6px;border:6px solid #fff;border-radius:50%;animation:lds-ring 1.2s cubic-bezier(.5,0,.5,1) infinite;border-color:#fff transparent transparent transparent}.lds-ring div:nth-child(1){animation-delay:-.45s}.lds-ring div:nth-child(2){animation-delay:-.3s}.lds-ring div:nth-child(3){animation-delay:-.15s}@keyframes lds-ring{0%{transform:rotate(0)}100%{transform:rotate(360deg)}}#loader{margin-left:auto;margin-right:auto}</style>
</head>
<body>
	<h1 id="smartWeather"class="animate-top">Smart<br>&nbsp; &nbsp; &nbsp;Register</h1><br>
	<h1 id="smartWeatherConnect" style="display:none;" class="animate-top">Smart<br>&nbsp; &nbsp; &nbsp;Register</h1><br>

	<div style = "display:none;" id = "loaderMessage">
		<h3 id = "Message"></h3>
		<form>
    		<input type = "button" value = "Reload in 5.." onclick = "reload()" id = "buttonConnect">
		</form>
	</div>
        
	<div id="loader" style="display:none;" class="animate-bottom">
		<h3 id="loaderTry"></h3>
		<div class="lds-ring">
	<div></div><div></div><div></div><div></div></div></div>
        
        
	<form id="ssidForm" class="animate-bottom">
)=====";