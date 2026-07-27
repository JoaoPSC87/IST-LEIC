describe('Shift', () => {
  const selectDateFromPicker = (inputSelector, wrapperSelector, date, referenceDate) => {
    const day = date.getDate()
    const referenceDay = referenceDate.getDate()

    // In case the day we want to select is in the next month
    if (referenceDay > day) {
      cy.get(inputSelector).click({ force: true })
      cy.get(wrapperSelector)
        .find('.datepicker-next:visible')
        .first()
        .click({ force: true })
      cy.wait(500)
    }

    cy.get(inputSelector).click({ force: true })
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
    cy.demoMemberLogin()

    cy.intercept('GET', '/users/*/getInstitution').as('activities')
    cy.intercept('GET', '/activities/*/shifts').as('activityShifts')

    // Navigate to shifts page
    cy.get('[data-cy="institution"]').click()
    cy.get('[data-cy="activities"]').click()
    cy.wait('@activities')
    cy.get('[data-cy="manageShifts"]').first().click()
    cy.wait('@activityShifts')

    // Open create shift dialog
    cy.get('[data-cy="createShift"]').should('not.be.disabled').click()
  });

  afterEach(() => {
    cy.logout();
    cy.deleteAllButArs()
  });

  it('save button is disabled when location is too short', () => {
    cy.get('[data-cy="locationInput"]').type('Short')
    cy.get('[data-cy="participantsLimitInput"]').type('3')

    // Pick start date
    cy.get('#startTimeInput-input').click({ force: true })
    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(startDate.getDate() + 2);
    const endDate = new Date(now);
    endDate.setDate(endDate.getDate() + 3);

    selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
    selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
    cy.get('body').type('{esc}')

    // Save should be disabled
    cy.get('[data-cy="saveShift"]').should('be.disabled')
  });

  it('save button is enabled when location has at least 20 characters', () => {
    cy.get('[data-cy="locationInput"]').type('This is a valid location for shift')
    cy.get('[data-cy="participantsLimitInput"]').type('3')

    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(startDate.getDate() + 2);
    const endDate = new Date(now);
    endDate.setDate(endDate.getDate() + 3);

    selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
    selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
    cy.get('body').type('{esc}')

    // Save should be enabled
    cy.get('[data-cy="saveShift"]').should('not.be.disabled')
  });

  it('shows error when shift start date is after end date', () => {
    cy.intercept('POST', '/activities/*/shift').as('createShift')

    cy.get('[data-cy="locationInput"]').type('This is a valid location for shift')
    cy.get('[data-cy="participantsLimitInput"]').type('3')

    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(startDate.getDate() + 3);
    const endDate = new Date(now);
    endDate.setDate(endDate.getDate() + 2);

    selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
    selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
    cy.get('body').type('{esc}')

    cy.get('[data-cy="saveShift"]').should('not.be.disabled').click()
    cy.wait('@createShift').then(({ response }) => {
      expect(response.statusCode).to.eq(400)
      expect(response.body.message).to.contain('Shift start time must be before end time')
    })

    cy.contains('.v-alert', 'Shift start time must be before end time').should('be.visible')
  });

  it('shows error when shift dates are outside activity date range', () => {
    cy.intercept('POST', '/activities/*/shift').as('createShift')

    cy.get('[data-cy="locationInput"]').type('This is a valid location for shift')
    cy.get('[data-cy="participantsLimitInput"]').type('3')

    //Pick dates far in the future, outside activity date range (activity ends at now+4)
    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(startDate.getDate() + 10);
    const endDate = new Date(now);
    endDate.setDate(endDate.getDate() + 11);

    selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
    selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
    cy.get('body').type('{esc}')

    cy.get('[data-cy="saveShift"]').should('not.be.disabled').click()
    cy.wait('@createShift').then(({response}) => {
      expect(response.statusCode).to.eq(400)
      expect(response.body.message).to.contain('Shift dates must be within activity date range')
    })

    cy.contains('.v-alert', 'Shift dates must be within activity date range').should('be.visible')
  });

  it('shows error when shift participants limit exceeds activity limit', () => {
    cy.intercept('POST', '/activities/*/shift').as('createShift')

    cy.get('[data-cy="locationInput"]').type('This is a valid location for shift')
    // Activity has participants_number_limit = 5 and already has a shift with limit 1
    // Setting 10 will exceed the total limit
    cy.get('[data-cy="participantsLimitInput"]').type('10')

    const now = new Date();
    const startDate = new Date(now);
    startDate.setDate(startDate.getDate() + 2);
    const endDate = new Date(now);
    endDate.setDate(endDate.getDate() + 3);

    selectDateFromPicker('#startTimeInput-input', '#startTimeInput-wrapper.date-time-picker', startDate, now)
    selectDateFromPicker('#endTimeInput-input', '#endTimeInput-wrapper.date-time-picker', endDate, now)
    cy.get('body').type('{esc}') 
    
    cy.get('[data-cy="saveShift"]').should('not.be.disabled').click()
    cy.wait('@createShift').then(({ response }) => {
      expect(response.statusCode).to.eq(400)
      expect(response.body.message).to.contain('Total participants of shifts exceeds activity limit')
    })

    cy.contains('.v-alert', 'Total participants of shifts exceeds activity limit').should('be.visible')
  });

  it('create button is disabled when activity is not approved', () => {
    cy.intercept('GET', '/users/*/getInstitution').as('activities')
    cy.intercept('GET', '/activities/*/shifts').as('activityShifts')

    cy.get('[data-cy="institution"]').click()
    cy.get('[data-cy="activities"]').click()
    cy.wait('@activities')

    cy.get('[data-cy="suspendButton"]').first().click()
    cy.get('[data-cy="suspensionReasonInput"]').type('Suspending activity to test disabled shift creation button')
    cy.get('[data-cy="suspendActivity"]').click()

    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .should('contain', 'SUSPENDED')

    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .contains('td', 'SUSPENDED')
      .parents('tr')
      .find('[data-cy="manageShifts"]')
      .click()
    cy.wait('@activityShifts')

    cy.get('[data-cy="createShift"]').should('be.disabled')
  });
});