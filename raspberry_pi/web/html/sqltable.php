<!doctype html>
<html>
<head>
    <meta charset = "UTF-8">
    <meta http-equiv="refresh" content = "30">
    <style type = "text/css">
        .spec{
            text-align:center;
        }
        .con{
            text-align:left;
        }
    </style>
</head>
<body>
    <hl align = "center">My Datbase</h1>
    <div class = "spec">
        #<b>The sensor value desciption></b><br>
        # 1 ~ 99 humi <br>
        # 1 ~ 99 temp

    <table border = '1' style = "width = 30 %" align = "center">
    <tr align = "center">
        <th>ID</th>
        <th>DATE</th>
        <th>TIME</th>
        <th>CDS</th>
        <th>SOUND</th>
    </tr>

    <?php
    $conn = mysqli_connect("localhost","root","raspberry");
    mysqli_select_db($conn,"test");
    $result = mysqli_query($conn, "select * from sensing");

    while($row = mysqli_fetch_array($result)){
        echo"<tr align = center>";
        echo '<td>'.$row['ID'].'</td>';
        echo '<td>'.$row['DATE'].'</td>';
        echo '<td>'.$row['TIME'].'</td>';
        echo '<td>'.$row['CDS'].'</td>';
        echo '<td>'.$row['SOUND'].'</td>';
        echo '</tr>';
        mysqli_close($conn);
    }
    ?>
    </table>
</body>
</html>