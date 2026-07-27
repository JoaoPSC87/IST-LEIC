<template>
  <v-card class="table">
    <div class="text-h3">{{ activity.name }}</div>
    <v-data-table
      :headers="headers"
      :items="shifts"
      :search="search"
      disable-pagination
      :hide-default-footer="true"
      :mobile-breakpoint="0"
      data-cy="activityShiftsTable"
    >
      <template v-slot:top>
        <v-card-title>
          <v-text-field
            v-model="search"
            append-icon="search"
            label="Search"
            class="mx-2"
          />
          <v-spacer />
                    <v-btn
            color="primary"
            dark
            @click="createShiftDialog = true"
            :disabled="activity.state !== 'APPROVED'"
            data-cy="createShift"
            >Create Shift</v-btn
          >
          <v-btn
            color="primary"
            dark
            @click="getActivities"
            data-cy="getActivities"
            >Activities</v-btn
          >
        </v-card-title>
      </template>
    </v-data-table>
    <shift-dialog
      v-if="createShiftDialog"
      v-model="createShiftDialog"
      :activity="activity"
      @close-shift-dialog="createShiftDialog = false"
      @save-shift="saveShift"
    />
  </v-card>
</template>
<script lang="ts">
import { Component, Vue } from 'vue-property-decorator';
import RemoteServices from '@/services/RemoteServices';
import Activity from '@/models/activity/Activity';
import Shift from '@/models/shift/Shift';
import ShiftDialog from '@/views/member/ShiftDialog.vue';

@Component({
  components: { ShiftDialog },
})
export default class InstitutionActivityShiftsView extends Vue {
  activity!: Activity;
  shifts: Shift[] = [];
  search: string = '';
  createShiftDialog: boolean = false;

  headers: object = [
    { text: 'Participants Limit', value: 'participantsLimit', align: 'left', width: '20%' },
    { text: 'Location', value: 'location', align: 'left', width: '30%' },
    { text: 'Start Time', value: 'formattedStartTime', align: 'left', width: '25%' },
    { text: 'End Time', value: 'formattedEndTime', align: 'left', width: '25%' },
  ];

  async created() {
    this.activity = this.$store.getters.getActivity;
    if (this.activity !== null && this.activity.id !== null) {
      await this.$store.dispatch('loading');
      try {
        this.shifts = await RemoteServices.getActivityShifts(this.activity.id);
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
      await this.$store.dispatch('clearLoading');
    }
  }

  async saveShift(shift: Shift) {
    this.shifts.push(shift);
    this.createShiftDialog = false;
  }

  async getActivities() {
    await this.$store.dispatch('setActivity', null);
    this.$router.push({ name: 'institution-activities' }).catch(() => {});
  }
}
</script>