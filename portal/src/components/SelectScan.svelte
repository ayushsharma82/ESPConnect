<script>
  import { createEventDispatcher } from 'svelte';
  const dispatch = createEventDispatcher();

  export let access_points;

</script>

<div class="container d-flex flex-columns">
  <div class="row h-100">
    {#if access_points.length > 0}
      <div class="column" style="margin-bottom: 3rem;">
        {#each access_points as ap}
          <div class="row clickable-row" on:click={() => dispatch('select', { ssid: ap.name, open: ap.open })}>
            <div class="column">
              <div class="container">
                <div class="row flex-rows">
                  <div class="column">
                    { ap.name }
                  </div>
                  <div class="column w-auto">
                    {#if !ap.open}
                      <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" style="fill: currentColor"><path d="M20,12c0-1.103-0.897-2-2-2h-1V7c0-2.757-2.243-5-5-5S7,4.243,7,7v3H6c-1.103,0-2,0.897-2,2v8c0,1.103,0.897,2,2,2h12 c1.103,0,2-0.897,2-2V12z M9,7c0-1.654,1.346-3,3-3s3,1.346,3,3v3H9V7z"></path></svg>
                    {/if}
                  </div>
                </div>
              </div>
            </div>
          </div>
        {/each}
      </div>
    {:else}
      <div class="column text-center text-muted">
        <p style="margin: 2rem 1rem;">
          <svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 24 24" style="fill: currentColor"><path d="M17.671,14.307C16.184,12.819,14.17,12,12,12s-4.184,0.819-5.671,2.307l1.414,1.414c1.11-1.11,2.621-1.722,4.257-1.722 c1.636,0.001,3.147,0.612,4.257,1.722L17.671,14.307z"></path><circle cx="12" cy="18" r="2"></circle></svg>
        </p>
        <p>
          No access points in range
        </p>
      </div>
    {/if}
  </div>
  <div class="row">
    <div class="column">
      <button class="button w-100" on:click={() => dispatch('refresh')}>Refresh</button>
    </div>
  </div>
</div>