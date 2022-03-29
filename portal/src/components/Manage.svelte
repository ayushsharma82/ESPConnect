<script>
  import { onMount, createEventDispatcher } from 'svelte';
  const dispatch = createEventDispatcher();

  export let settingsFields;
  let loading = false;

  async function getSettings(){
    loading = true;
    let formData = new FormData();
    settingsFields.forEach(field => {
      formData.append(field.id, "");  
    });
    //const res = await fetch(`/espconnect.json`);
    const res = await fetch(`/espconnect/getSettings`, { method: 'POST', body: formData });
		if (res.status === 200) {
      let data = await res.json();
      if (data){
        data.forEach((dataElement) => {
          let keys = Object.keys(dataElement);
          let values = Object.values(dataElement);
          let field = settingsFields.find((x) => x.id === keys[0]);
          if (field) {
            field.val = values[0];
          }
        });
      }

      settingsFields = settingsFields; // required to force update
		} else {
      dispatch('error');
    }
    loading = false;
		return res;
  }

  async function save(){
    loading = true;
    let formData = new FormData();
    settingsFields.forEach(field => {
      formData.append(field.id, field.val);  
    });
    const res = await fetch(`/espconnect/saveSettings`, { method: 'POST', body: formData });
		if (res.status === 200) {
      back();
		} else {
      dispatch('error');
    }
    loading = false;
		return res;
  }

  function back(){
    dispatch('back')
  }

  onMount(async () => {
    loading = true;
    await getSettings();
    loading = false;
  })
</script>

<form class="container d-flex flex-columns" on:submit|preventDefault={save}>
  <div class="row h-100">
    <div class="column">
      <div class="row">
        <div class="column text-center">
          <p class="mb-2 text-muted">
            Manage Configuration
          </p>
        </div>
      </div>
      {#each settingsFields as field}
        <div class="row">
          <div class="column column-100">
            <label for="server">{field.label}</label>
            <input type="text" placeholder={field.ph} id={field.id} bind:value={field.val} required>
          </div>
        </div>
      {/each}
    </div>
  </div>
  <div class="row flex-rows">
    <div class="column w-auto pr-1">
      <button type="button" class="button btn-light text-center" on:click={back} disabled={loading}>
        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" style="fill: #fff; vertical-align: middle"><path d="M21 11L6.414 11 11.707 5.707 10.293 4.293 2.586 12 10.293 19.707 11.707 18.293 6.414 13 21 13z"></path></svg>
      </button>
    </div>
    <div class="column">
      <button type="submit" class="button w-100 text-center" disabled={loading}>
        {#if loading}
          <div class="btn-loader"></div>
        {:else}
          Save
        {/if}
      </button>
    </div>
  </div>
</form>