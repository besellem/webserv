<html>
<body>
    <?php if ($_SERVER["REQUEST_METHOD"] == "POST") { ?>
    Welcome <?php echo $_POST["name"]; ?><br>
    Your email address is: <?php echo $_POST["email"]; ?>
    <?php }
          else{ ?>
     <script> location.replace("yourHtmlFileName.html") </script>
     <?php     } ?>
</body>
</html> 