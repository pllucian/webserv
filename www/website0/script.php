<?php
    $name = "anonymous";
    $age = "no age";
    parse_str(getenv("QUERY_STRING"),$data);
    if (isset($data["name"]))
        $name = htmlspecialchars($data["name"]);
    if (isset($data["age"]))
        $age = htmlspecialchars($data["age"]);
    $content_type = getenv("CONTENT_TYPE");
?>
<!
<html>
<head>
    <title>Welcome to website0!</title>
    <meta charset="utf-8">
    <style>
        body {
            width: 35em;
            margin: 0 auto;
            font-family: Tahoma, Verdana, Arial, sans-serif;
        }
    </style>
</head>
<body>
<h1>Welcome to website0!</h1>
<?php
    if ($content_type === "multipart/form-data")
        echo "<p>File upload is successful!</p>";
    else
    {
        echo "<p>Name: $name</p>";
        echo "<p>Age: $age</p>";
    }
?>
    <p><a href="/">back</a></p>
</body>
</html>
