//TODO : Its a Completed Code
function formvalid() {
  var vaildpass = document.getElementById("passId").value;

  if (vaildpass.length < 8 || vaildpass.length > 20) {
    document.getElementById("vaild-pass").innerHTML = "Minimum 8 characters";
    return false;
  } else {
    document.getElementById("vaild-pass").innerHTML = "";
  }
}

function show() {
  var x = document.getElementById("passId");
  if (x.type === "password") {
    x.type = "text";
    document.getElementById("showimg").src =
      "show.png";
  } else {
    x.type = "password";
    document.getElementById("showimg").src =
      "hide.png";
  }
}
// validasi user dan pass
function check(form) {
    const user = "admin";
    const pass = "Admin123";
    if (form.userId.value == user && form.passId.value == pass) {
      window.open('upload.html')
      console.log("berhasil");
    } else {
      console.log("login gagal");
    }
}
