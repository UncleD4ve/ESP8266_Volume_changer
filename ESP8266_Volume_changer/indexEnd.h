const char indexEnd[] PROGMEM = R"=====(
    <h6>Made by David Friday</h6> 
</form>
<script>
var state = 0;
var lastTry = 0;
var timer = [];
var interval;

function getSSID(value){
	document.getElementById("SSID").value=value;
}

function clearTimer(){
	for(var i = 0;i < timer.length;i++)
		clearTimeout(timer[i]);
	timer.length = 0;
}

function connect()
{
  api = "connect" + "?ssid=" +
  document.getElementById("SSID").value + "&pass=" +
  document.getElementById("Password").value + "&key=19";

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      connectionPage();
    }
  };
  xhttp.open("GET", api, true);
  xhttp.send();
}

function reload()
{
  location.reload();
}

function connectionPage() { 
	state = 1; lastTry = 0; interval = setInterval(checkStatus, 1000);
	timer.push(setTimeout(function() {loaderMessage("Time out!");}, 22000));
	document.getElementById("ssidForm").style.display = "none";
	document.getElementById("loader").style.display = "block";
	document.getElementById("loaderTry").innerHTML=("try 1/10");
	document.getElementById("smartWeather").style.display = "none";
	document.getElementById("smartWeatherConnect").style.display = "block";
}

function checkStatus(){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			console.log(this.responseText);
			if (this.responseText[0] === 'W') 
			{
    			let trys = this.responseText.substring(1);
				if(trys > lastTry)
					document.getElementById("loaderTry").innerHTML=("try "+trys+"/10");
				lastTry = trys;

    			if(trys == 1)
				{
        			clearTimer();
					timer.push(setTimeout(function() {loaderMessage("Time out!");}, 25000));
				}
			}
			else if (this.responseText[0] === 'I') 
			{
				clearTimer();
				loaderMessage("Connected successfully!<br><br>IP Address: "+this.responseText.substring(1));
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) 
					document.getElementById("Message").innerHTML = this.responseText;
				};
				xhttp.open("GET", "restart", true);
				xhttp.send();
			}
			else if (this.responseText[0] === 'N') 
			{
				clearTimer();
				loaderMessage("Wrong credentials!");
			}
			else 
			  console.log('Unknown event');
		}
	};
	xhttp.open("GET", "status", true);
	xhttp.send();
}

function loaderMessage(message) {
	clearInterval(interval);
	document.getElementById("Message").innerHTML = message;
	document.getElementById("loader").style.display = "none";
	document.getElementById("loaderMessage").style.display = "block";
    var x = document.getElementById("buttonConnect");
	x.value="Reload in 5.."; state = 0;
    setTimeout(function(){ x.value="Reload in 4.." }, 1000);
  	setTimeout(function(){ x.value="Reload in 3.." }, 2000);
  	setTimeout(function(){ x.value="Reload in 2.." }, 3000);
  	setTimeout(function(){ x.value="Reload in 1.." }, 4000);
    setTimeout(function(){ x.value="Reload in 0..";location.reload();}, 5000);
}
</script>
</body>
</html>
)=====";