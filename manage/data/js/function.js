function LightImageChange() {
    var httpRequest = new XMLHttpRequest();
    httpRequest.open('GET', "./relay_control", true);
    httpRequest.send();
    if (httpRequest.readyState == 4 && httpRequest.status == 200) {
        var json = JSON.parse(httpRequest.responseText);
        if (json['status'] == '1') {
            light.setAttribute('src', './static/on.png');
        } else if (json['status'] == '0'){
            light.setAttribute('src', './static/off.png')
        }
    } else {
        return;
    }
}