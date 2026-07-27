describe('Enrollment', () => {
  const selectDateFromPicker = (inputString ,wrapperSelector, date, referenceDate) => {
    const day = date.getDate()
    const referenceDay = referenceDate.getDate()

    // In case the day we want to select is in the next month
    if (referenceDay > day) {
      cy.get(inputString).click({ force: true })
      cy.get(wrapperSelector)
        .find('.datepicker-next:visible')
        .first()
        .click({ force: true })
      cy.wait(500) 
    }
    cy.get(inputString).click({ force: true })
    cy.get(wrapperSelector)
      .find('.datepicker-day-text')
      .contains(new RegExp(`^\\s*${day}\\s*$`))
      .first()
      .click({ force: true })
  }

  beforeEach(() => {
    cy.deleteAllButArs()
    cy.createDemoEntities();
    cy.createDatabaseInfoForEnrollments()
  });

  afterEach(() => {
    cy.deleteAllButArs()
  });

  it('create enrollment', () => {
    const MOTIVATION = 'I am very keen to help other people.';

    cy.intercept('GET', '/users/*/getInstitution').as('activities');
    cy.intercept('POST', '/enrollments').as('enroll');
    cy.intercept('GET', '/activities/1/enrollments').as('enrollments');
    cy.intercept('GET', '/activities/1/shifts').as('shifts');


    // Member login and manage shifts
    cy.demoMemberLogin()
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();
    cy.wait('@activities')

    cy.get('[data-cy="memberActivitiesTable"] thead th').then(($headers) => {
      const applicationsColIdx = [...$headers].findIndex(
        (header) => header.textContent?.trim() === 'Applications',
      )
      expect(applicationsColIdx).to.be.greaterThan(-1)
      cy.wrap(applicationsColIdx).as('applicationsColIdx')
    })
    
    // Verify there are 3 activities and store initial enrollment count for first activity
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .should('have.length', 3)

    cy.get('@applicationsColIdx').then((applicationsColIdx) => {
      cy.get('[data-cy="memberActivitiesTable"] tbody tr')
        .eq(0)
        .children()
        .eq(Number(applicationsColIdx))
        .invoke('text')
        .then((text) => {
          const initialEnrollmentCount = parseInt((text.match(/\d+/) || ['0'])[0], 10)
          expect(Number.isNaN(initialEnrollmentCount)).to.be.false
          cy.wrap(initialEnrollmentCount).as('initialEnrollmentCount')
        })
    })
    
    // Open shifts management for first activity
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .find('[data-cy="manageShifts"]')
      .click()
    cy.wait('@shifts')
    
    // Create two more shifts
    for (let i = 1; i < 3; i++) {
      cy.get('[data-cy="createShift"]').click()
      cy.get('[data-cy="locationInput"]').type(`Shift Location - New Shift ${i + 1}`)
      cy.get('[data-cy="participantsLimitInput"]').type('1')
      
      const now = new Date();
      const startDate = new Date(now);
      startDate.setDate(startDate.getDate() + 2);
      const endDate = new Date(now);
      endDate.setDate(endDate.getDate() + 3);

      selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
      selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
      cy.get('body').type('{esc}')

      cy.get('[data-cy="saveShift"]').click()
    }
    
    cy.get('[data-cy="getActivities"]').click()
    cy.wait('@activities')
    cy.logout();

    // volunteer login, creates enrollment with 2 of 3 shifts
    cy.demoVolunteerLogin()
    cy.get('[data-cy="volunteerActivities"]').click();
    cy.get('[data-cy="volunteerActivitiesTable"] tbody tr')
      .eq(0)
      .find('[data-cy="applyButton"]').click();
    
    cy.get('[data-cy="motivationInput"]').type(MOTIVATION);
    
    // Select first and second shifts (skip the third)
    cy.get('[data-cy="shiftsSelect"]').click()
    cy.get('.v-menu__content.menuable__content__active:visible')
      .last()
      .within(() => {
        cy.get('.v-list-item:visible').eq(0).click()
        cy.get('.v-list-item:visible').eq(1).click()
      })

    cy.get('body').type('{esc}')
    
    cy.get('[data-cy="saveEnrollment"]').click()
    cy.wait('@enroll')
    cy.logout()

    // member login and check enrollment count
    cy.demoMemberLogin()
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    cy.get('@applicationsColIdx').then((applicationsColIdx) => {
      cy.get('[data-cy="memberActivitiesTable"] tbody tr')
        .eq(0)
        .children()
        .eq(Number(applicationsColIdx))
        .invoke('text')
        .then((text) => {
          const updatedEnrollmentCount = parseInt((text.match(/\d+/) || ['0'])[0], 10)
          expect(Number.isNaN(updatedEnrollmentCount)).to.be.false
          cy.get('@initialEnrollmentCount').then((initialEnrollmentCount) => {
            expect(updatedEnrollmentCount).to.eq(Number(initialEnrollmentCount) + 1)
          })
        })
    })

    // open enrollments view for first activity
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .find('[data-cy="showEnrollments"]').click()
    cy.wait('@enrollments');
    // check that there are 1 enrollment
    cy.get('[data-cy="activityEnrollmentsTable"] tbody tr')
      .should('have.length', 1)
      .eq(0)
      .children()
      .should('have.length', 7)
      .eq(1)
      .should('contain', MOTIVATION)

    cy.logout();
  });

  it('update an enrollment', () => {
    const MOTIVATION2 = 'Motivation Example Two';
    
    // volunteer login and edit an enrollment
    cy.demoVolunteerLogin()
    cy.get('[data-cy="volunteerEnrollments"]').click()
    cy.get('[data-cy="volunteerEnrollmentsTable"] tbody tr')
    .eq(0)
    .find('[data-cy="updateEnrollmentButton"]').click()
    
    cy.get('[data-cy="motivationInput"]').clear().type(MOTIVATION2);
    cy.get('[data-cy="saveEnrollment"]').click()
    cy.logout();

    // member check if the motivation is the new one
    cy.demoMemberLogin()
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(1)
      .find('[data-cy="showEnrollments"]').click()

    cy.get('[data-cy="activityEnrollmentsTable"] tbody tr')
    .eq(0)
    .children()
    .eq(1)
    .should('contain', MOTIVATION2)
    
    cy.logout();

  });

  it('delete an enrollment', () => {
    const ACTIVTIY_NAME_ONE = 'A2';

    // volunteer delete an enrollment
    cy.demoVolunteerLogin()
    cy.get('[data-cy="volunteerEnrollments"]').click()
    cy.get('[data-cy="volunteerEnrollmentsTable"] tbody tr')
    .eq(0)
    .children()
    .eq(0).should('contain', ACTIVTIY_NAME_ONE);

    cy.get('[data-cy="volunteerEnrollmentsTable"] tbody tr')
    .eq(0)
    .find('[data-cy="deleteEnrollmentButton"]').click();
    
    // check where is the actvity that volunteer delete enrollment and check that we can enroll again
    cy.get('[data-cy="volunteerActivities"]').click();
    cy.get('[data-cy="volunteerActivitiesTable"] tbody tr')
    .eq(1)
    .children()
    .eq(0)
    .should('contain', ACTIVTIY_NAME_ONE);

    cy.get('[data-cy="volunteerActivitiesTable"] tbody tr')
    .eq(1)
    .children()
    .eq(10)
    .find('[data-cy="applyButton"]')
    .should('exist');

    cy.logout();

    // member verify that this Activity don't have enrollments
    cy.demoMemberLogin()
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(1)
      .find('[data-cy="showEnrollments"]').click()
    
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .should('have.length', 0)

    cy.logout();
    
  });

  it('save button is not visible when selected shifts overlap', () => {
    cy.intercept('GET', '/users/*/getInstitution').as('activities')
    cy.intercept('GET', '/activities/*/shifts').as('shifts')

    // Member creates 2 shifts with the same time range (they overlap)
    cy.demoMemberLogin()
    cy.get('[data-cy="institution"]').click()
    cy.get('[data-cy="activities"]').click()
    cy.wait('@activities')

    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0).find('[data-cy="manageShifts"]').click()
    cy.wait('@shifts')

    for (let i = 1; i <= 2; i++) {
      cy.get('[data-cy="createShift"]').click()
      cy.get('[data-cy="locationInput"]').type(`Overlapping Shift Location ${i}`)
      cy.get('[data-cy="participantsLimitInput"]').type('1')

      const now = new Date()
      const startDate = new Date(now)
      startDate.setDate(startDate.getDate() + 2)
      const endDate = new Date(now)
      endDate.setDate(endDate.getDate() + 3)

      selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
      selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
      cy.get('body').type('{esc}')

      cy.get('[data-cy="saveShift"]').click()
    }

    cy.get('[data-cy="getActivities"]').click()
    cy.wait('@activities')
    cy.logout()

    //Volunteer selects both overlapping shifts
    cy.demoVolunteerLogin()
    cy.get('[data-cy="volunteerActivities"]').click()
    cy.get('[data-cy="volunteerActivitiesTable"] tbody tr')
      .eq(0).find('[data-cy="applyButton"]').click()
    
    cy.get('[data-cy="motivationInput"]').type('I am very keen to help other people.')

    cy.get('[data-cy="shiftsSelect"]').click()
    cy.get('.v-menu__content.menuable__content__active:visible')
      .last()
      .within(() => {
        cy.get('.v-list-item:visible').eq(1).click()
        cy.get('.v-list-item:visible').eq(2).click()
      })

    cy.get('body').type('{esc}')

    // Save button should not exist because shifts overlap
    cy.get('[data-cy="saveEnrollment"]').should('not.exist')

    cy.logout()
  });
});