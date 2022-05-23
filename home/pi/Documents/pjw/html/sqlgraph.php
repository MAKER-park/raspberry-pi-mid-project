<?php
    $conn = mysqli_connect("localhost","root","raspberry");
    mysqli_select_db($conn,"test");
    $result = mysqli_query($conn, "select DATE, TIME, SOUND from sensing LIMIT 30");
    $data = array(array('test','소리'));
    if($result){
        while($row = mysqli_fetch_array($result)){
            array_push($data,array($row['DATE']."\n".$row[1] , intval($row[2])));
        }
    }
    $options = array(
        'title' => '소리 (단위 : dB)',
        'width' => 1000, 'height'=>500
    );
?>

<script src="//www.google.com/jsapi"></script>
<script>
    let data = <?= json_encode($data) ?>;
    let options = <?= json_encode($options) ?>;
    google.load('visualization', '1.0', {'packages':['linechart']});
    google.setOnLoadCallback(function() {
        let chart = new google.visualization.ColumnChart(document.querySelector('#chart_div'));
        chart.draw(google.visualization.arrayToDataTable(data), options);
    });
</script>
<div id="chart_div"></div>