<template>
  <v-dialog v-model="dialog" persistent width="1300">
    <v-card>
      <v-card-title>
        <span class="headline">New Shift</span>
      </v-card-title>
      <v-card-text>
        <v-form ref="form" lazy-validation>
          <v-row>
            <v-col cols="12">
              <v-text-field
                label="*Location"
                required
                v-model="editShift.location"
                data-cy="locationInput"
                :rules = "locationRules"
                counter="200"
              ></v-text-field>
            </v-col>
            <v-col cols="12" sm="6" md="4">
              <v-text-field
                label="*Participants Limit"
                required
                v-model="editShift.participantsLimit"
                data-cy="participantsLimitInput"
              ></v-text-field>
            </v-col>
            <v-col>
              <VueCtkDateTimePicker
                id="startTimeInput"
                v-model="editShift.startTime"
                format="YYYY-MM-DDTHH:mm:ssZ"
                label="*Start Time"
                data-cy="startTimeInput"
              ></VueCtkDateTimePicker>
            </v-col>
            <v-col>
              <VueCtkDateTimePicker
                id="endTimeInput"
                v-model="editShift.endTime"
                format="YYYY-MM-DDTHH:mm:ssZ"
                label="*End Time"
                data-cy="endTimeInput"
              ></VueCtkDateTimePicker>
            </v-col>
          </v-row>
        </v-form>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn
          color="blue-darken-1"
          variant="text"
          @click="$emit('close-shift-dialog')"
          data-cy="cancelShift"
        >
          Close
        </v-btn>
        <v-btn
          :disabled="!canSave"
          color="blue-darken-1"
          variant="text"
          @click="createShift"
          data-cy="saveShift"
        >
          Save
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script lang="ts">
import { Vue, Component, Prop, Model } from 'vue-property-decorator';
import Activity from '@/models/activity/Activity';
import Shift from '@/models/shift/Shift';
import RemoteServices from '@/services/RemoteServices';
import VueCtkDateTimePicker from 'vue-ctk-date-time-picker';
import 'vue-ctk-date-time-picker/dist/vue-ctk-date-time-picker.css';

Vue.component('VueCtkDateTimePicker', VueCtkDateTimePicker);

@Component
export default class ShiftDialog extends Vue {
  @Model('dialog', Boolean) dialog!: boolean;
  @Prop({ type: Activity, required: true }) readonly activity!: Activity;

  editShift: Shift = new Shift();
  
  locationRules = [
    (v: string) => !!v || 'Location is required',
    (v: string) => (v && v.length >= 20) || 'Location must be at least 20 characters',
    (v: string) => (v && v.length <= 200) || 'Location must be at most 200 characters', 
  ];

  get canSave(): boolean {
    return (
      !!this.editShift.location &&
      this.editShift.location.length >= 20 &&
      this.editShift.location.length <= 200 &&
      !!this.editShift.participantsLimit &&
      !!this.editShift.startTime &&
      !!this.editShift.endTime
    );
  }

  async createShift() {
    if (this.activity.id !== null) {
      try {
        const result = await RemoteServices.createShift(
          this.activity.id,
          this.editShift,
        );
        this.$emit('save-shift', result);
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
  }
}
</script>

<style scoped lang="scss"></style>
