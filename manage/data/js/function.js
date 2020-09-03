function LightImageChange() {
    var tag = document.getElementById('light');
    if (tag.getAttribute('src') == './static/on.png') {
        tag.setAttribute('src', './static/off.png');
    } else {
        tag.setAttribute('src', './static/on.png');
    }
    var httpRequest = new XMLHttpRequest();
    httpRequest.open('GET', './relay_control', false);
    httpRequest.send(true);
}


function RefreshData() {
    var httpRequest = new XMLHttpRequest();
    httpRequest.open('GET', './info', false);
    httpRequest.send(true);
    console.log(httpRequest.status);

    if (httpRequest.status == 200) {
        var json = JSON.parse(httpRequest.responseText);
        if (json['msg'] != 'ERROR') {
            var relayStatusTag = document.getElementById('light');
            var temperTag = document.getElementById('temperature');
            var humidityTag = document.getElementById('humidity');

            if (json['relay'] == '0' && relayStatusTag.getAttribute('src') != './static/off.png') {
                relayStatusTag.setAttribute('src', './static/off.png');
            } else if (json['relay'] == '1' && relayStatusTag.getAttribute('src') != './static/on.png') {
                relayStatusTag.setAttribute('src', './static/on.png');
            }
            if (json['temperature'] != undefined)
                temperTag.innerText = json['temperature'];
            if (json['humidity'] != undefined)
                humidityTag.innerText = json['humidity'];
            // console.log('logloglog');
        }
    }
}

/**
 *  动态显示当前时间
 */
function showDateTime(){
    var sWeek=new Array("日","一","二","三","四","五","六");  //声明数组存储一周七天
    var myDate=new Date(); //获取当天日期
    var sYear=myDate.getFullYear(); //获取年
    var sMonth=myDate.getMonth()+1; //获取月
    var sDate=myDate.getDate(); //获取日
    var sDay=sWeek[myDate.getDay()]; //根据得到的数字星期,利用数组转化为星期
    var h=myDate.getHours(); //获取小时
    var m=myDate.getMinutes(); //获取分钟
    var s=myDate.getSeconds(); //获取秒
    //输入日期和星期
    document.getElementById("date").innerHTML=(sYear+"年"+sMonth+"月"+sDate+"日"+"星期"+sDay+"<br/>");
    h = formatTwoDigits(h);  //格式化小时,如果不足两位在前面补0
    m = formatTwoDigits(m); //格式化分钟,如果不足两位在前面补0
    s = formatTwoDigits(s); //格式化秒钟后,如果不足两位在前面补0
    //显示时间
    document.getElementById("msg").innerHTML=(h+":"+m+":"+s+"<br/>");
    
    // 刷新 温度字体
    var temperTag = document.getElementById("temperature")
    var temperature = Number(temperTag.innerText);
    if (temperature <= 0) {
        temperTag.setAttribute("style", "color: rgb(38, 59, 116);");
    } else if (temperature > 0 && temperature <= 15) {
        temperTag.setAttribute("style", "color: rgb(30, 159, 210);");
    } else if (temperature > 15 && temperature <= 26) {
        temperTag.setAttribute("style", "color: rgb(30, 201, 87);");
    } else if (temperature > 26 && temperature <= 30) {
        temperTag.setAttribute("style", "color: rgb(201, 198, 30);");
    } else if (temperature > 30 && temperature <= 40) {
        temperTag.setAttribute("style", "color: rgb(221, 60, 11);");
    } else {
        temperTag.setAttribute("style", "color: red;");
    }
    setTimeout("showDateTime()",1000);//每秒执行一次showDateTime函数
}
//如果输入数是一位数,则在十位上补0
function formatTwoDigits(s) {
    if (s<10)
        return "0"+s;
    else
        return s;
}

window.onload=function() {
    showDateTime();
    RefreshData();
};
setInterval("RefreshData()", 5000);