<?php
    $conn = mysqli_connect("localhost","root","raspberry");
    mysqli_select_db($conn,"project");
    $result = mysqli_query($conn, "select DATE, TEMP, HUM from tank LIMIT 30");
    $data = array(array('project','온도','습도'));
    if($result){
        while($row = mysqli_fetch_array($result)){
            array_push($data,array($row['DATE']."\n".$row[0] , intval($row[1]), intval($row[2])));
        }
    }
    $options = array(
        'title' => '온도 (단위 : °C), 습도 (단위 : %)',
        'width' => 1000, 'height'=>500
    );
?>
<link href="style.css" rel="stylesheet" type="text/css">
<meta http-equiv="refresh" content = "5">
</head>
<script src="//www.google.com/jsapi"></script>
<script>
    let data = <?= json_encode($data) ?>;
    let options = <?= json_encode($options) ?>;
    google.load('visualization', '1.0', {'packages':['corechart']});
    google.setOnLoadCallback(function() {
        let chart = new google.visualization.LineChart(document.querySelector('#chart_div'));
        chart.draw(google.visualization.arrayToDataTable(data), options);
    });

</script>
<div id="chart_div"></div>