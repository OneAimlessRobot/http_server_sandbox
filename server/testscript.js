function start(){

	let testText=document.getElementById("test");
	testText.title= "Teste";
	testText.textContent="TESTE TESTE TESTE???!!!"
	mesg("clicaste");
	
}
function submit(){

	let textSubmited=document.getElementById("testForm");
	let textshower=document.getElementById("testshower");

	testshower.textContent=textSubmitted.value;
	mesg("clicaste 2");

}
