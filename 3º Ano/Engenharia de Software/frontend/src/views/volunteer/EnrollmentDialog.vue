<template>
  <v-dialog v-model="dialog" persistent width="800">
    <v-card>
      <v-card-title>
        <span class="headline">
          {{
            editEnrollment && editEnrollment.id === null
              ? 'New Application'
              : 'Edit Application'
          }}
        </span>
      </v-card-title>
      <v-card-text>
        <v-form ref="form" lazy-validation>
          <v-row>
            <v-col cols="12">
              <v-textarea
                label="*Motivation"
                :rules="[(v) => !!v || 'Motivation is required']"
                required
                v-model="editEnrollment.motivation"
                data-cy="motivationInput"
                auto-grow
                rows="1"
              ></v-textarea>
            </v-col>
          </v-row>
          <v-row v-if="availableShifts.length > 0">
            <v-col cols="12">
              <v-select
                label="*Select Shifts"
                :items="availableShifts"
                :item-text="formatShift"
                :item-value="(item) => item.id"
                v-model="editEnrollment.shiftIds"
                multiple
                chips
                required
                :rules="[(v) => (v && v.length > 0) || 'At least one shift is required',
                () => !hasOverlappingShifts || 'Selected shifts have overlapping periods'
                ]"
                data-cy="shiftsSelect"
              ></v-select>
            </v-col>
          </v-row>
        </v-form>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn
          color="blue-darken-1"
          variant="text"
          @click="$emit('close-enrollment-dialog')"
        >
          Close
        </v-btn>
        <v-btn
          v-if="canSave"
          color="blue-darken-1"
          variant="text"
          @click="updateEnrollment"
          data-cy="saveEnrollment"
        >
          Save
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>
<script lang="ts">
import { Vue, Component, Prop, Model, Watch } from 'vue-property-decorator';
import RemoteServices from '@/services/RemoteServices';
import { ISOtoString } from '@/services/ConvertDateService';
import Enrollment from '@/models/enrollment/Enrollment';
import Shift from '@/models/shift/Shift';

@Component({
  methods: { ISOtoString },
})
export default class EnrollmentDialog extends Vue {
  @Model('dialog', Boolean) dialog!: boolean;
  @Prop({ type: Enrollment, required: true }) readonly enrollment!: Enrollment;

  editEnrollment: Enrollment = new Enrollment();
  availableShifts: Shift[] = [];

  async created() {
    this.editEnrollment = new Enrollment(this.enrollment);
    if (this.editEnrollment.activityId) {
      await this.loadShifts();
    }
  }

  @Watch('enrollment', { deep: true })
  async onEnrollmentChange() {
    this.editEnrollment = new Enrollment(this.enrollment);
    if (this.editEnrollment.activityId) {
      await this.loadShifts();
    }
  }

  async loadShifts() {
    try {
      if (this.editEnrollment.activityId !== null) {
        this.availableShifts = await RemoteServices.getActivityShifts(
          this.editEnrollment.activityId,
        );
      }
    } catch (error) {
      await this.$store.dispatch('error', error);
    }
  }

  formatShift(shift: Shift): string {
    return `${shift.location} (${shift.formattedStartTime} - ${shift.formattedEndTime})`;
  }

  get hasOverlappingShifts(): boolean {
    const selected = this.availableShifts.filter(
      s => s.id !== null && this.editEnrollment.shiftIds.includes(s.id!)
    );
    for (let i = 0; i < selected.length; i++) {
      for (let j = i + 1; j < selected.length; j++) {
        const a = selected[i];
        const b = selected[j];
        if (new Date(a.startTime) < new Date(b.endTime) &&
            new Date(b.startTime) < new Date(a.endTime)) {
          return true;
        }
      }
    }
    return false;
  }

  get canSave(): boolean {
    return (
      !!this.editEnrollment.motivation &&
      this.editEnrollment.motivation.length >= 10 &&
      this.editEnrollment.shiftIds &&
      this.editEnrollment.shiftIds.length > 0 &&
      !this.hasOverlappingShifts
    );
  }

  async updateEnrollment() {
    //editar
    if (
      this.editEnrollment.id !== null &&
      (this.$refs.form as Vue & { validate: () => boolean }).validate()
    ) {
      try {
        const result = await RemoteServices.editEnrollment(
          this.editEnrollment.id,
          this.editEnrollment,
        );
        this.$emit('update-enrollment', result);
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
    //criar
    else if (
      this.editEnrollment.activityId !== null &&
      (this.$refs.form as Vue & { validate: () => boolean }).validate()
    ) {
      try {
        const result = await RemoteServices.createEnrollment(
          this.editEnrollment,
        );
        this.$emit('save-enrollment', result);
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
    }
  }
}
</script>

<style scoped lang="scss"></style>
